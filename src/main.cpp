#include <iostream>
#include <random>
#include <chrono>
#include "include/field.hpp"
#include "include/useful.hpp"

using namespace std::chrono;

inline double get_time_sec(void){
    return static_cast<double>(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count())/1000000000;
}

int main(int argc, char *argv[]) {
	/*
	Field test(10, 15);

	test.print();	

	for(size_t i = 0; i < 10; i++) {
		test.testMoveAgent();
		test.print();
	}
	*/

	uint_fast64_t buf;
	size_t i;
	double start, end;
	size_t datanum = 1e5;

	std::vector<double> data(datanum, 0);

	std::random_device seed;
	XorOshiro128p oshiro(seed());

	start = get_time_sec();
	for(size_t j = 0; j < 10; j++) {
		for(i = 0; i < datanum; i++) {
			data[i] += oshiro.gend();
		}
	}
	end = get_time_sec();
	std::cout << "xoroshiro+: " << end - start << std::endl;

	DPlot<double> plot(data);
	plot.histogram();
	
	/*
	DPlot<uint_fast64_t> dp(data);
	dp.histogram("r");
	*/

	return 0;
}
