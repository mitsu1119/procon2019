#include <iostream>
#include <random>
#include <chrono>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

int main(int argc, char *argv[]) {
	
	Field field(20, 20);

	DisplayWrapper* test = new Display();
	
	test->setField(&field);
	test->setInstance(test);
	test->start(argc, argv);

	delete test;
	
	/*Field test(10, 15);

	test.print();	

	for(size_t i = 0; i < 20; i++) {
		test.testMoveAgent();
		test.print();
	}
	*/
	return 0;
}
