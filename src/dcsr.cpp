/*
DOUBLY COMPRESSED SPARSE ROWS
*/
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <new>
#include <tuple>
#include <cblas.h>
#include <algorithm>				//Checking if v contains the element x
#include <iterator>
//#include </home/feynmann/block_sparse/include/dcsc.h>

std::string which_major;
int leading_dim, laggging_dim;
typedef std::vector<std::tuple<int, int, double>> my_tuple;

double randNum() {// initialize random seed:
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0, 9);
	//Use dis to transform the random unsigned int generated by gen into a double in [0,1]
	//Each call to dis(gen) generates a new random double
	auto randx = dis(gen);        //get random x0
	//      std::cout << "Random X " << randx << std::endl;
	return randx;
}

void printVec (const std::vector<auto> vec) {
	for (auto i : vec){	//pythonic range-based for loops
		std::cout << i << "\t";
	}
	std::cout << std::endl;
}

void random_denseMat(const auto lead_dim, const auto lag_dim, auto &mat_A){
	for (int i = 0, ii = 0; i != lag_dim; ++i) {
		for (int j = 0; j != lead_dim; ++j, ++ii) {
			auto tmpA = rand()%10;
			mat_A[ii] = tmpA;
		}
	}
}

void diagonalMatrix(const auto lead_dim, const auto lag_dim, auto &mat_A){
	for (int i = 0, ii=0; i != lag_dim; ++i) {
		for (int j = 0; j != lead_dim; ++j, ++ii) {
			if(i==j){
				auto tmpA = randNum();
				mat_A[ii] = tmpA;			//row-major
			}
			else
				mat_A[ii] = 0;
		}
	}
}

void transpose(const auto lead_dim, const auto lag_dim, const std::vector<auto> matrix, std::vector<auto> &transpose_matrix) {
	//this simply changes data layout	
	for (int i = 0, ii = 0; i != lag_dim; ++i) {
		for (int j = 0;  j != lead_dim; ++j, ++ii) {
			transpose_matrix[j*lead_dim + i] = matrix[ii];
		}
	}
}

void get_dim_from_major (const auto lead_dim, const auto lag_dim, auto &rows, auto &cols) {
	if (which_major == "row-major" || which_major == "rowmajor") {
		rows = lag_dim;
		cols = lead_dim;
	}
	else if (which_major == "col-major" || which_major == "colmajor") {
		rows = lead_dim;
		cols = lag_dim;
	}
	else {
		std::cout << "which major? you suck" << std::endl;
	}
}

void printMatrix (const auto lead_dim, const auto lag_dim, const auto msg, const auto matrix) {
	std::cout << msg << std::endl;
	int rows, cols;
	get_dim_from_major(lead_dim, lag_dim, rows, cols);
	//std::cout << "rows " << rows << std::endl;
	//std::cout << "cols " << cols << std::endl;
	for (int i = 0; i != rows; ++i) {
		for (int j = 0; j != cols; ++j) {
			if (which_major == "row-major" || which_major == "rowmajor") {
				std::cout << matrix[i*rows + j] << "\t";
			}
			else if (which_major == "col-major" || which_major == "colmajor") {
				std::cout << matrix[j*rows + i] << "\t";
			}
		} std::cout << std::endl;
	}
}

void print_tuples (auto vector_of_tuples) {
	//print tuples
	for (my_tuple::const_iterator i = vector_of_tuples.begin(); i != vector_of_tuples.end(); ++i) {
		std::cout << std::get<0> (*i) << "\t ";
		std::cout << std::get<1> (*i) << "\t ";
		std::cout << std::get<2> (*i) << std::endl;
	}
}

void check_major (const auto m, const auto n, auto &leading_dim, auto &lagging_dim) {
	std::cout << which_major << std::endl;
	if (which_major == "row-major" || which_major == "rowmajor"){
		leading_dim = n;						//row-major => leading_dim = #cols
		lagging_dim = m;
	}
	else if (which_major == "col-major"|| which_major == "colmajor") {
		leading_dim = m;						//col-major => leading_dim = #rows
		lagging_dim = n;	
	}
}

void convert_triples_to_dcsc(const int rows, const int cols, const auto triples_A, auto &val, auto &indices, auto &cp_dcsc, auto &jc_dcsc, auto &aux, auto &nnz, auto &nzc, auto &upper_bound_cf) {
	std::vector <int> row_idx, col_idx, jc_csc;
	for (my_tuple::const_iterator i = triples_A.begin(); i != triples_A.end(); ++i) {
		row_idx.push_back(std::get<0>(*i));
		col_idx.push_back(std::get<1>(*i));
		if (which_major == "col-major"|| which_major == "colmajor"){
			int r = std::get<0>(*i);
			int c = std::get<1>(*i);
			indices.push_back(r);
			if (std::find(jc_dcsc.begin(), jc_dcsc.end(), c) == jc_dcsc.end()) {	//if elem not found
				jc_dcsc.push_back(c);
			}
		}	
		else if (which_major == "row-major" || which_major == "rowmajor"){
			indices.push_back(std::get<1>(*i));
			jc_dcsc.push_back(std::get<0>(*i));
		}
		val.push_back(std::get<2>(*i));
	}

	nnz = val.size();
	
	int cnt = 0;
	//chunk size cf = (n+1)/nzc :: nzc = number of nonzero cols
	nzc = jc_dcsc.size();
	
	double cf = (double) (cols+1)/nzc;
	std::cout << "cf " << cf << std::endl;
	upper_bound_cf = ceil(cf);
	
	
	for (int i = 0; i != cols; ++i){
		for (int j = 0; j != nnz; ++j) {
			if (j == 0){
				jc_csc.push_back(cnt);
			}
			if (j == 0 && std::find(cp_dcsc.begin(), cp_dcsc.end(), cnt) == cp_dcsc.end()) {
				cp_dcsc.push_back(cnt);
			}
			if (col_idx[j] == i) {
				++cnt;
			}
		}
	}
	jc_csc.push_back(nnz);			//size of jc_csc = cols + 1
	
	//pushback #nnz to cp
	cp_dcsc.push_back(nnz);
	//std::cout << "csc format jc" << std::endl;
	//printVec(jc_csc);
	
	//AUXILLIARY ARRAY LOGIC
	std::vector <int> d, cp;
	int cnt_aux = 0, cnt_cf = 0;
	bool flag_cf = true;
	for (int i = 0; i != cols+1; ++i){
		//std::cout << "cnt_cf " << cnt_cf << std::endl;
		//std::cout << "cnt_aux " << cnt_aux << std::endl;
		int tmp = jc_csc[i+1] - jc_csc[i];
		d.push_back(tmp);
		if (tmp != 0) {
			cp.push_back(jc_csc[i]);
			if (flag_cf == true) {
				aux.push_back(cnt_aux);
				flag_cf = false;
			}
			++cnt_aux;
		}

		++cnt_cf;
		if (cnt_cf % upper_bound_cf == 0) {
			flag_cf = true;
			cnt_cf = 0;
		}
		
	}

	std::cout << "diff arr" << std::endl;
	printVec(d);
	std::cout << "last element of d " << d[cols] << std::endl;
	cp.push_back(nnz);
	//std::cout << "cp arr" << std::endl;
	//printVec(cp);
	
	//pushback total number of cols to aux
	aux.push_back(nnz);
	//std::cout << "aux" << std::endl;
	//printVec(aux);
}

void get_sorted_indices(const auto lead_dim, const auto lag_dim, const auto sparse_matrix, auto &val, auto &indices) {
	std::vector <int> idx_ptr, cp_arr, csc_jc, jc_new;
	int cnt = 0;
	bool flag_csc_jc = false, flag_cp_arr = false;
	for(int i = 0, ii=0; i != lag_dim; ++i){
		for(int j = 0; j != lead_dim; ++j, ++ii){
			if (j==0) {
				//flag_csc_jc = true;
				flag_cp_arr = true;
			}
			idx_ptr.push_back(cnt);
			if(sparse_matrix[ii] != 0){
				val.push_back(sparse_matrix[ii]);
				indices.push_back(j);				//indices contain col_ind if rowmajor, row_ind if col-major	
				jc_new.push_back(i);				//jc contains cols/rows that contain atleast one nnz element
				if (flag_cp_arr == true) {
					cp_arr.push_back(cnt);
					flag_cp_arr = false;
				}
				cnt++;
			}
		} 
	}
	int nnz = val.size();
	std::cout << "nnz " << nnz << std::endl;
	idx_ptr.push_back(nnz);
	cp_arr.push_back(nnz);
	int cf;
	int rows, cols;
	get_dim_from_major(lead_dim, lag_dim, rows, cols);
	cf = (cols + 1)/nnz;
	std::cout << "cf " << cf << std::endl;
	std::cout << "cp_arr" << std::endl;
	printVec(cp_arr);
	std::cout << "old csc jc array" << std::endl;
	printVec(csc_jc);
	//std::cout << "row ptr in csr storage" << std::endl;
	//printVec(row_ptr);

}

void access_elem_in_matrix(const auto &i, const auto &j, const auto nnz, const auto nzc, const auto cf, const auto &val, const auto &ir, const auto &jc, const auto &cp, const auto &aux, const auto &rows, const auto &cols) {
	/*
	std::vector <int> vec = {4,2,1,3,5,6,2,9,3,1};
	auto starts = vec.begin();
	auto ende = vec.begin() + 3;
	for (auto it = starts; it <= ende; it++) {
		//std::cout << "value in vec " << *it << std::endl;	//gives value at that point in vector
		//std::cout << "idx in vec " << distance(starts,it) << std::endl;
		if (*it == 3) {
			std::cout << "first code: elem found" << std::endl;
			std::cout << "value in vec " << *it << std::endl;	//gives value at that point in vector
			std::cout << "idx in vec " << distance(starts,it) << std::endl;
		}
	}
	auto find_elem = std::find(starts, ende, 3);
	if (find_elem != ende) {
		std::cout << "FOUND" << std::endl;
		std::cout << "value in vec " << *find_elem << std::endl;	//gives value at that point in vector
		std::cout << "idx in vec " << distance(starts,find_elem) << std::endl;
	}
	else {
		std::cout << "second code " << std::endl;
		std::cout << "element not found " << std::endl;
	}
	*/
	if (i > rows || j > cols) {
		std::cout << "Error : Index provided is greater than matrix dimensions." << std::endl;
	}
	else{
		std::cout << "searching for element A(" << i << ", " << j << ")..." << std::endl;
		int idx = floor(j/cf);
		int s = aux[idx];
		int e = aux[idx+1];
		std::cout << "cf " << cf << std::endl;
		//std::cout << "start " << s << " end " << e << std::endl;
		//std::cout << "jc" << std::endl;
		printVec(jc);

		auto start = jc.begin() + s;
		auto end = jc.begin() + e;
		
		int pos, posc;
		auto iter_col = std::find(start, end, j);
		if (iter_col != end) {
			pos = distance(jc.begin(), iter_col);
			//std::cout << "found value j = " << j << " at index " << "jc[" << distance(jc.begin(), iter_col) << "]" << std::endl;
			//std::cout << "found value j = " << j << " at index " << "jc[" << pos << "]" << std::endl;
			
			int sc = cp[pos];
			int ec = cp[pos+1];
			//std::cout << "startc " << sc << " endc " << ec << std::endl;
			auto startc = ir.begin() + sc;
			auto endc = ir.begin() + ec;
			
			std::cout << "ir" << std::endl;
			printVec(ir);

			auto iter_row = std::find (startc, endc, i);
			if (iter_row != endc) {
				posc = distance(ir.begin(), iter_row);
				//std::cout << "Found element " << i << " in " << "ir[" << distance(ir.begin(), iter_row) << "]" << std::endl;
				//std::cout << "found element " << i << " in " << "ir[" << posc << "]" << std::endl;
				std::cout << "Element found! Value is " << val[posc] << std::endl;
			}
			else {
				std::cout << "Element is 0" << std::endl;
			}
		}
		else 
			std::cout << "Element is 0" << std::endl;
	}
}
/*int main (int argc, char *argv[]) {
	//int m = atoi(argv[2]);
	//int n = atoi(argv[3]);
	
	int m, n;
	m = n = 6;
	which_major = argv[1];
	int lead_dim, lag_dim;					//for both matrices but two diff for multiplication
	check_major(m, n, lead_dim, lag_dim);
	//std::cout << "row/col major? " << which_major << std::endl;
	//std::cout << "leading_dim " << lead_dim << " lag_dim " << lag_dim << std::endl;
	int rows, cols;
	get_dim_from_major(lead_dim, lag_dim, rows, cols);
	std::cout << "rows " << rows << " cols " << cols << std::endl;

	//std::vector<double> sparse = {1, 2, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 3, 0, 5, 16};
	std::vector<double> sparse = {1, 2, 0, 0, 0, 0, 4, 0, 3, 0.5, 0, 2};
	std::vector<double> A = sparse;
	//printMatrix(lead_dim, lag_dim, "matrix A", A);
	
	std::vector <double> val_A;
	std::vector <int> idx;
	//get_sorted_indices(lead_dim, lag_dim, A, val_A, idx);				//get col indices of A									//matrix stored in row major
	
	//given triples
	my_tuple triples_A;
	triples_A.push_back(std::make_tuple(5,0,0.1));
	triples_A.push_back(std::make_tuple(7,0,0.2));
	triples_A.push_back(std::make_tuple(3,6,0.3));
	triples_A.push_back(std::make_tuple(1,7,0.4));

	//9*9 matrix
	my_tuple eg_A;
	eg_A.push_back(std::make_tuple(0,0,0.2));
	eg_A.push_back(std::make_tuple(0,1,0.3));
	eg_A.push_back(std::make_tuple(1,1,0.4));
	eg_A.push_back(std::make_tuple(8,5,0.1));

	//4*6 matrix
	my_tuple whiteboard;
	whiteboard.push_back(std::make_tuple(2,0,1));
	whiteboard.push_back(std::make_tuple(2,2,2));
	whiteboard.push_back(std::make_tuple(0,3,3));
	whiteboard.push_back(std::make_tuple(2,4,4));

	my_tuple beamer; //6*6 matrix
	beamer.push_back(std::make_tuple(0,1,1));
	beamer.push_back(std::make_tuple(3,1,2));
	beamer.push_back(std::make_tuple(2,2,3));
	beamer.push_back(std::make_tuple(5,5,4));

	//sort tuples before passing it to convert_dcsc functions
	//std::cout << "unsorted tuple" << std::endl;
	//print_tuples(dense1);
	std::sort(begin(beamer), end(beamer), 
    	[](std::tuple<int, int, double> const &t1, std::tuple<int, int, double> const &t2) {
      	  return std::get<1>(t1) < std::get<1>(t2);
   		}
	);
	//std::cout << "sorted tuple" << std::endl;
	//print_tuples(dense1);

	std::vector <int> ir, cp_dcsc, jc_dcsc, aux;
	std::vector <double> num_dcsc;
	int nnz, nzc, cf;
	convert_triples_to_dcsc(rows, cols, beamer, num_dcsc, ir, cp_dcsc, jc_dcsc, aux, nnz, nzc, cf);

	std::cout << "DCSC REPRESENTATION" << std::endl;
	std::cout << "nnz " << nnz << std::endl;
	std::cout << "nzc " << nzc << std::endl;
	std::cout << "upper cf " << cf << std::endl;
	std::cout << "num" << std::endl;
	printVec(num_dcsc);
	std::cout << "ir" << std::endl;
	printVec(ir);
	std::cout << "jc" << std::endl;
	printVec(jc_dcsc);
	std::cout << "cp" << std::endl;
	printVec(cp_dcsc);
	std::cout << "aux" << std::endl;
	printVec(aux);
	
	access_elem_in_matrix(0, 4, nnz, nzc, cf, num_dcsc, ir, jc_dcsc, cp_dcsc, aux, rows, cols);
	return 0;
}*/