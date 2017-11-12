cmake_minimum_required (VERSION 3.6) 
project (compressed_row_storage) 

set(CMAKE_CXX_STANDARD 14)
add_executable(crs crs.cpp)
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
