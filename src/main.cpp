#include <iostream>
#include <random>
#include <chrono>
#include "field.hpp"
#include "useful.hpp"

int main(int argc, char *argv[]) {
	Field test(10, 15);

	test.print();	

	for(size_t i = 0; i < 50; i++) {
		test.testMoveAgent();
		test.print();
	}

	return 0;
}
