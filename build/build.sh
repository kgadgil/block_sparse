#!/bin/sh

rm -rf CMakeFiles/ CMakeCache.txt
cmake ../src
make crs VERBOSE=1
./crs 4