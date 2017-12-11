/*
Print Functions Header file
*/
#include <iostream>
#include <vector>
void printVec (const std::vector<auto> vec) {
	for (auto i : vec){	//pythonic range-based for loops
		std::cout << i << "\t";
	}
	std::cout << std::endl;
}