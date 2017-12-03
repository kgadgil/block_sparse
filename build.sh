#!/bin/sh

#export MKL_INCLUDE_DIRS="/opt/intel/compilers_and_libraries_2018/linux/mkl/include/"
#export MKL_LIBRARIES="/opt/intel/compilers_and_libraries_2018/linux/mkl/lib/"
cd bin
rm -rf CMakeFiles/ CMakeCache.txt
rm dcsr
cmake ../src/
#make crs VERBOSE=1
make dcsr VERBOSE=1
#make spgemm VERBOSE=1
#./crs col-major 3 4
./dcsr col-major 9 9
#./spgemm col-major