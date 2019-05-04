#include <iostream>
#include <random>
#include <chrono>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

int main(int argc, char *argv[]) {
	
	Field field(10, 15);

	DisplayWrapper* test = new Display();
	
	test->setInstance(test);
	test->setField(&field);
	test->start(argc, argv);
	
	/*
	Field test(10, 15);

	test.print();	

	for(size_t i = 0; i < 20; i++) {
		test.testMoveAgent();
		test.print();
	}
	*/

	return 0;
}
