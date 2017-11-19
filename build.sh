#!/bin/sh

#export MKL_INCLUDE_DIRS="/opt/intel/compilers_and_libraries_2018/linux/mkl/include/"
#export MKL_LIBRARIES="/opt/intel/compilers_and_libraries_2018/linux/mkl/lib/"
cd bin
rm -rf CMakeFiles/ CMakeCache.txt
cmake ../src/
#make crs VERBOSE=1
make dcsr VERBOSE=1
#./crs 4
./dcsr