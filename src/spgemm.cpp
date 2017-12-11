/*
SpGemm
Buluc, A., & Gilbert, J. (2011). Parallel Sparse Matrix-Matrix Multiplication and Indexing: Implementation and Experiments, 94720. https://doi.org/10.1137/110848244
*/

#include <doubly_compress.h>

int main (int argc, char *argv[]) {
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
}