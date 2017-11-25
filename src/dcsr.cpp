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

std::string which_major;
int leading_dim, laggging_dim;
typedef std::vector<std::tuple<int, int, double>> my_tuple;

double randNum() {/* initialize random seed: */
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0, 9);
	//Use dis to transform the random unsigned int generated by gen into a double in [0,1]
	//Each call to dis(gen) generates a new random double
	auto randx = dis(gen);        //get random x0
	//      std::cout << "Random X " << randx << std::endl;
	return randx;
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

void printVec (const std::vector<auto> vec) {
	for (auto i : vec){	//pythonic range-based for loops
		std::cout << i << "\t";
	}
	std::cout << std::endl;
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

void isect (const std::vector<auto> idx_A, const std::vector<auto> idx_B, std::vector<auto> &isect_vec) {			//intersection of A.JC and BT.JC; set of indices that contribute non-trivially to outer product
	for (int i = 0; i!= idx_A.size(); ++i){
		for (int j = 0; j!= idx_B.size(); ++j){
			if(idx_A[i] == idx_B[j])							//get ONLY elements common only once
				isect_vec.push_back(idx_A[i]);
		}
	}	
}

void convert_triples_to_csc(const int rows, const int cols, const auto triples_A, auto &val, auto &indices, auto &jc_csc) {
	std::vector <int> row_idx, col_idx;
	for (my_tuple::const_iterator i = triples_A.begin(); i != triples_A.end(); ++i) {
		row_idx.push_back(std::get<0>(*i));
		col_idx.push_back(std::get<1>(*i));
		if (which_major == "col-major"|| which_major == "colmajor"){
			indices.push_back(std::get<0>(*i));
		}	
		else if (which_major == "row-major" || which_major == "rowmajor"){
			indices.push_back(std::get<1>(*i));
		}
		val.push_back(std::get<2>(*i));
	}

	int nnz = val.size();
	int cnt = 0;
	for (int i = 0; i != cols; ++i){				//pythonic range-based for loops
		for (int j = 0; j != nnz; ++j) {
			if (j==0) {
				jc_csc.push_back(cnt);
			}
			if (col_idx[j] == i) {
				++cnt;
			}
		}
	}
	jc_csc.push_back(val.size());
}

void convert_triples_to_dcsc(const int rows, const int cols, const auto triples_A, auto &val, auto &indices, auto &cp_dcsc, auto &jc_dcsc, auto &aux) {
	std::vector <int> row_idx, col_idx, jc_csc;
	for (my_tuple::const_iterator i = triples_A.begin(); i != triples_A.end(); ++i) {
		row_idx.push_back(std::get<0>(*i));
		col_idx.push_back(std::get<1>(*i));
		if (which_major == "col-major"|| which_major == "colmajor"){
			indices.push_back(std::get<0>(*i));
			jc_dcsc.push_back(std::get<1>(*i));
		}	
		else if (which_major == "row-major" || which_major == "rowmajor"){
			indices.push_back(std::get<1>(*i));
			jc_dcsc.push_back(std::get<0>(*i));
		}
		val.push_back(std::get<2>(*i));
	}
	int nnz = val.size();
	std::cout << "nnz " << nnz << std::endl;
	int cnt = 0;
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
	jc_csc.push_back(nnz);
	std::cout << "csc format jc" << std::endl;
	printVec(jc_csc);
	std::vector <int> diff_arr;
	for (int i = 0; i != jc_csc.size()-1; ++i){
		int tmp = jc_csc[i+1] - jc_csc[i];
		diff_arr.push_back(tmp);
	}
	std::cout << "diff arr" << std::endl;
	printVec(diff_arr);
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

int main (int argc, char *argv[]) {
	//int m, n;
	//m = n = 4;
	int m = atoi(argv[2]);
	int n = atoi(argv[3]);
	which_major = argv[1];
	int lead_dim, lag_dim;					//for both matrices but two diff for multiplication
	check_major(m, n, lead_dim, lag_dim);
	//std::cout << "row/col major? " << which_major << std::endl;
	//std::cout << "leading_dim " << lead_dim << " lag_dim " << lag_dim << std::endl;
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

	//functionality to sort triples before/after func
	my_tuple eg_A;
	eg_A.push_back(std::make_tuple(0,0,0.2));
	eg_A.push_back(std::make_tuple(0,1,0.3));
	eg_A.push_back(std::make_tuple(1,4,0.4));
	eg_A.push_back(std::make_tuple(8,5,0.1));

	std::vector <int> col_ptrs, indices;
	std::vector <double> num;
	convert_triples_to_csc(9, 9, triples_A, num, indices, col_ptrs);
	/*std::cout << "num" << std::endl;
	printVec(num);
	std::cout << "row indices" << std::endl;
	printVec(indices);
	std::cout << "jc_csc_ptrs" << std::endl;
	printVec(col_ptrs);
	*/
	std::vector <int> idx_dcsc, cp_dcsc, jc_dcsc, aux;
	std::vector <double> num_dcsc;

	convert_triples_to_dcsc(9, 9, triples_A, num_dcsc, idx_dcsc, cp_dcsc, jc_dcsc, aux);
	/*std::cout << "num" << std::endl;
	printVec(num_dcsc);
	std::cout << "jc_dcsc" << std::endl;
	printVec(jc_dcsc);
	std::cout << "cp_dcsc" << std::endl;
	printVec(cp_dcsc);
	*/	
	return 0;
}