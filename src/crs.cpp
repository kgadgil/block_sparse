/*
COMPRESSED STORAGE
*/

#include <iostream>
#include <vector>
#include <random>
#include <new>
#include <typeinfo>

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

void printMatrix (int rows, int cols, auto msg, auto matrix, auto which_major) {
	std::cout << msg << std::endl;
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

void printVec (const std::vector<auto> vec) {
	for (int i = 0; i < vec.size(); i++){
		std::cout << vec[i] << std::endl;
	}
}

void compressed_storage (const auto lead_dim, const auto lag_dim, auto sparse_matrix) {						//pass in vector
	std::vector <double> val;
	//std::vector <int> col_ind, row_ind;
	std::vector <int> indices, idx_ptr;
	//std::vector <std::vector<double>> key_val_pair;
	int cnt = 0;
	bool new_r_c = false;
	for(int i = 0, ii=0; i != lag_dim; ++i){				//dim+1 to get next val of row_idx
		for(int j = 0; j != lead_dim; ++j, ++ii){
			if (j==0) 
				new_r_c = true;
			if (new_r_c == true) {
					//row_ptr.push_back(cnt);
					idx_ptr.push_back(cnt);
					if (i==lag_dim && j == lead_dim){
						idx_ptr.push_back(cnt);
					}
					new_r_c = false;
			}
			if(sparse_matrix[ii] != 0){
				val.push_back(sparse_matrix[ii]);
				//row_ind.push_back(i);
				//col_ind.push_back(j);
				indices.push_back(j);				//indices contain col_ind if rowmajor, row_ind if col-major	
				++cnt;
			}
		}
	}
	int nnz = val.size();
	std::cout << "nnz " << nnz << std::endl;
	idx_ptr.push_back(nnz);
	/*for (int j = 0; j != lead_dim; ++j) {
		for (int k = idx_ptr[j]; k != idx_ptr[j+1]-1; ++k) {
			std::cout << "value of j " << j << std::endl; 
			std::cout << "vat dis k " << k << std::endl;
			auto v = val[k];
			std::cout << "value for this k " << v << std::endl; 
		}
	}
	*/
	/*
	key_val_pair.push_back(val);
	key_val_pair.push_back(row_ind);
	key_val_pair.push_back(col_ind);
	for (int i = 0; i < key_val_pair.size(); i++) {
		for (int j = 0; j < key_val_pair[1].size(); j++) {
			std::cout << key_val_pair[i][j] << std::endl;
		}
	}
	*/

	//location in val vector where new row starts
	/*int row_elem = val.size();
	for(int i = 0; i < row_ind.size(); i++){
		for(int j = 0; j < col_ind.size(); j++){
			row_ptr.push_back(val[i*row_elem + j]);			//FIX : logic for row_ptr
		}
	}
	*/
	

	std::cout << "idx_ptr" << std::endl;
	printVec(idx_ptr);
	std::cout << "indices" << std::endl;
	printVec(indices);
	std::cout << "val" << std::endl;
	printVec(val);
	/*std::cout << "column index" << std::endl;
	printVec(col_ind);
	std::cout << "row index" << std::endl;
	printVec(row_ind);
	std::cout << "row ptr" << std::endl;
	printVec(row_ptr);
	*/
}

int main (int argc, char *argv[]) {
	int m = atoi(argv[2]);
	int n = atoi(argv[3]);

	int lead_dim, lag_dim;					//SET LEADING DIMENSION
	std::string leading = argv[1];
	std::cout << leading << std::endl;
	if (leading == "row-major" || leading == "rowmajor"){
		lead_dim = n;						//row-major => leading_dim = #cols
		lag_dim = m;
	}
	else if (leading == "col-major"|| leading == "colmajor") {
		lead_dim = m;						//col-major => leading_dim = #rows
		lag_dim = n;	
	}
	std::cout << "m " << m << " n " << n << std::endl;
	std::cout << "lead_dim " << lead_dim << " lag_dim " << lag_dim << std::endl;
	/*double *denseA;										//dynamic array
	denseA = new double[m*n];
	random_denseMat(m, n, denseA);
	std::string msg_dense_mat = "dense matrix A";
	//print_ip_mat(m, n, msg_dense_mat, denseA);
	
	std::vector<double> diagA (m*n);					//vector; which is faster?
	std::string msg_diag_mat = "diagonal matrix A";
	diagonalMatrix(m, n, diagA);
	//print_ip_mat(m, n, msg_diag_mat, diagA);

	int length = m*n;
	std::vector<double> sp(length);
	double array [length];
	//Generate random sparse matrix
	int t = 6;											//target nnz elements
	for (int i = 0; i < t; ++i) {
		int index = (int) (length * (int) randNum());
		array[index] = i % 2 ? -1 : 1;
	}
	//print_ip_mat (m, n, "sparse_matrix", array);
	//end sparse

	double sparse [16] = {1, 2, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 3, 0, 5, 16};
	print_ip_mat(m, n, "sparse matrix manual", sparse);
	csr(m, n, sparse);											//matrix stored in row major
	delete [] denseA;
	*/
	std::vector<double> rm = {1, 0, 4, 0.5, 2, -1, 0, 0, 0, 0, 3, 2};
	std::vector<double> cm = {1, 2, 0, 0, 0, 0, 4, 0, 3, 0.5, 0, 2};
	printMatrix(m, n, "matrix A", cm, leading);
	compressed_storage (lead_dim, lag_dim, cm);
	return 0;
}