#include <iostream>
#include <random>
#include "include/field.hpp"
#include "include/useful.hpp"

int main(int argc, char *argv[]) {
	/*
	Field test(10, 15);

	test.print();	

	for(size_t i = 0; i < 10; i++) {
		test.testMoveAgent();
		test.print();
	}
	*/

	std::vector<uint_fast64_t> data;

	std::random_device seed;
	XorOshiro128p rand(seed());

	for(size_t i = 0; i < 10000; i++) data.push_back(rand.randull());

	DPlot<uint_fast64_t> dp(data);
	dp.histogram();

	return 0;
}
