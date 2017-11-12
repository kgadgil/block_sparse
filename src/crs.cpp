/*
COMPRESSED ROW STORAGE
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

void print_ip_mat (auto rows, auto cols, auto msg, const auto mat){
	std::cout << msg << std::endl;
	int cnt = 0;
	for (int i = 0; i != rows*cols; ++i){
		std::cout << mat[i] << "\t";
		cnt++;
		if(cnt == rows){		//row major: change to cols if col-major
			std::cout << std::endl;
			cnt = 0;
		}
	}
}

void printVec (const std::vector<auto> vec) {
	for (int i = 0; i < vec.size(); i++){
		std::cout << vec[i] << std::endl;
	}
}

void random_denseMat(const auto rows, const auto cols, auto &mat_A){
	for (int i = 0; i != rows; ++i) {
		for (int j = 0; j != cols; ++j) {
			auto tmpA = rand()%10;
			mat_A[i*cols + j] = tmpA;			//row-major
		}
	}

}

void diagonalMatrix(const auto rows, const auto cols, auto &mat_A){
	for (int i = 0; i != rows; ++i) {
		for (int j = 0; j != cols; ++j) {
			if(i==j){
				auto tmpA = randNum();
				mat_A[i*cols + j] = tmpA;			//row-major
			}
			else
				mat_A[i*cols + j] = 0;
		}
	}

}

void csr(const auto rows, const auto cols, auto sparse_matrix) {						//pass in vector
	std::vector <double> val;
	std::vector <int> col_ind, row_ptr;

	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if(sparse_matrix[i*cols + j]!=0){
				val.push_back(sparse_matrix[i*cols + j]);
				col_ind.push_back(j);
				row_ptr.push_back(i);
			}
		}
	}
	std::cout << "val" << std::endl;
	printVec(val);
	std::cout << "column index" << std::endl;
	printVec(col_ind);
	std::cout << "row ptr" << std::endl;
	printVec(row_ptr);
}

int main (int argc, char *argv[]) {
	int m = atoi(argv[1]);
	int n = atoi(argv[1]);

	double *denseA;										//dynamic array
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
	int t = 6;											//target nnz elements
	for (int i = 0; i < t; ++i) {
		int index = (int) (length * (int) randNum());
		array[index] = i % 2 ? -1 : 1;
	}
	print_ip_mat (m, n, "sparse_matrix", array);

	csr(m, n, diagA);											//matrix stored in row major


	delete [] denseA;
	return 0;
}