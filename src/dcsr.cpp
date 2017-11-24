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

void check_major (const auto m, const auto n, auto &leading_dim, auto &lagging_dim) {
	std::cout << which_major << std::endl;
	if (which_major == "row-major" || which_major == "rowmajor"){
		leading_dim = n;						//row-major => leading_dim = #cols
		lagging_dim = m;
	}
	else if (which_major == "col-major"|| which_major == "colmajor") {
		std::cout << "m " << m << std::endl;
		std::cout << "n " << n << std::endl;
		leading_dim = m;						//col-major => leading_dim = #rows
		lagging_dim = n;	
	}
}
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

void printVec (const std::vector<auto> vec) {
	for (int i = 0; i < vec.size(); i++){
		std::cout << vec[i] << "\t";
	}
	std::cout << std::endl;
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
	std::cout << "rows " << rows << std::endl;
	std::cout << "cols " << cols << std::endl;
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

void convert_triples_to_csc(auto triples_A) {
	std::vector <int> row_idx, col_idx;
	std::vector <double> val;
	for (my_tuple::const_iterator i = triples_A.begin(); i != triples_A.end(); ++i) {
		row_idx.push_back(std::get<0>(*i));
		col_idx.push_back(std::get<1>(*i));
		val.push_back(std::get<2>(*i));
	}
}
void get_sorted_indices(const auto lead_dim, const auto lag_dim, const auto sparse_matrix, auto &val, auto &indices) {
	std::vector <int> idx_ptr, cp_arr, csc_jc, jc_new;
	int cnt = 0;
	bool flag_csc_jc = false, flag_cp_arr = false;
	for(int i = 0, ii=0; i != lag_dim; ++i){
		for(int j = 0; j != lead_dim; ++j, ++ii){
			if (j==0) {
				flag_csc_jc = true;
				flag_cp_arr = true;
			}
			if (flag_csc_jc == true) {
					idx_ptr.push_back(cnt);
					flag_csc_jc = false;
			}
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

void isect (const std::vector<auto> idx_A, const std::vector<auto> idx_B, std::vector<auto> &isect_vec) {			//intersection of A.JC and BT.JC; set of indices that contribute non-trivially to outer product
	for (int i = 0; i!= idx_A.size(); ++i){
		for (int j = 0; j!= idx_B.size(); ++j){
			if(idx_A[i] == idx_B[j])							//get ONLY elements common only once
				isect_vec.push_back(idx_A[i]);
		}
	}	
}

int main (int argc, char *argv[]) {
	//int m, n;
	//m = n = 4;
	int m = atoi(argv[2]);
	int n = atoi(argv[3]);
	which_major = argv[1];
	int lead_dim, lag_dim;					//for both matrices but two diff for multiplication
	check_major(m, n, lead_dim, lag_dim);
	std::cout << "row/col major? " << which_major << std::endl;
	std::cout << "leading_dim " << lead_dim << " lag_dim " << lag_dim << std::endl;
	//std::vector<double> sparse = {1, 2, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 3, 0, 5, 16};
	std::vector<double> sparse = {1, 2, 0, 0, 0, 0, 4, 0, 3, 0.5, 0, 2};
	std::vector<double> A = sparse;
	printMatrix(lead_dim, lag_dim, "matrix A", A);
	
	std::vector <double> val_A;
	std::vector <int> idx;
	get_sorted_indices(lead_dim, lag_dim, A, val_A, idx);				//get col indices of A									//matrix stored in row major
	
	//given triples
	
	my_tuple triples_A;
	triples_A.push_back(std::make_tuple(5,0,0.1));
	triples_A.push_back(std::make_tuple(7,0,0.2));
	triples_A.push_back(std::make_tuple(3,6,0.3));
	triples_A.push_back(std::make_tuple(1,7,0.4));

	for (my_tuple::const_iterator i = triples_A.begin(); i != triples_A.end(); ++i) {
		std::cout << std::get<0> (*i) << "\t ";
		std::cout << std::get<1> (*i) << "\t ";
		std::cout << std::get<2> (*i) << std::endl;
	}
	convert_triples_to_csc(triples_A);
	return 0;
}