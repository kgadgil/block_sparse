cmake_minimum_required (VERSION 3.6) 
#project (compressed_row_storage) 
#project (doubly_compressed_sparse_rows)
project (dcsc_sparse_gemm)

set(CMAKE_CXX_STANDARD 14)
find_package(BLAS)

#add_executable(crs crs.cpp)
#add_executable(dcsr dcsr.cpp)
add_executable(spgemm spgemm.cpp)