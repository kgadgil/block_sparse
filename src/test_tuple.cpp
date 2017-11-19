/*
Test CPP tuple
*/

/*
g++ -std=c++14 src/test_tuple.cpp -o bin/test_tuple
*/

#include <iostream>
#include <vector>
#include <tuple>

std::tuple <double, int, int> add_elem_to_tuple () {
	std::tuple <double, int, int> elem;
	for (int i = 0; i <4; ++i){
		elem = std::make_tuple(2.00, i, i);	
	}
	return elem;
}
void print_tuple_val (std::tuple<double, int, int> tup) {		//convert to template that accepts any value of N
	std::cout << std::get<int>(tup) << std::endl;
}
void printVec (const std::vector<auto> vec) {
	for (int i = 0; i < vec.size(); i++){
	//	print_tuple_val(vec[i]);
	}
}

int main () {
	std::tuple<double, int, int> elem (2,1,1);
	auto element = std::make_tuple (2,1,1);
	//std::cout << std::get<0>(element) << std::endl;
	//std::cout << std::get<0>(elem) << std::endl;

	typedef std::tuple<double, int, int> T;
	auto part = std::make_tuple(2.1, 1, 1);
	
	std::vector<T> vec_of_tuples;
	T first = std::make_tuple(1,2,3);
	T second = std::make_tuple(99,101,111);
	vec_of_tuples.push_back(first);
	vec_of_tuples.push_back(second);
	printVec(vec_of_tuples);
	std::string tuple_str;
	print_tuple_val(part);
}