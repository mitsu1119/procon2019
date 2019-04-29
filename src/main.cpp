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

	std::vector<uint_fast64_t> data(1e7);
	uint_fast64_t buf;
	size_t i;
	double start, end;

	std::random_device seed;
	std::mt19937_64 engine(seed());

	start = get_time_sec();
	for(i = 0; i < 1e7; i++) {
		data[i] = engine();
	}
	end = get_time_sec();
	std::cout << "mt19937_64: " << end - start << std::endl;

	DPlot<uint_fast64_t> plot(data);
	plot.histogram();

	XorOshiro128p test(seed());

	start = get_time_sec();
	for(i = 0; i < 1e7; i++) {
		data[i] = test();
	}
	end = get_time_sec();
	std::cout << "xoroshiro+: " << end - start << std::endl;

	plot = DPlot<uint_fast64_t>(data);
	plot.histogram("r");
	
	/*
	DPlot<uint_fast64_t> dp(data);
	dp.histogram("r");
	*/

	return 0;
}
