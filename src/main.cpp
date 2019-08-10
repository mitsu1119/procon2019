#include <iostream>
#include <random>
#include <chrono>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

int main(int argc, char *argv[]) {
	
	Field field(10, 10);

	DisplayWrapper* test = new Display();
	
	test->setField(&field);
	test->setInstance(test);
	test->start(argc, argv);

	delete test;
	
	return 0;
}
