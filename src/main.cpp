#include <iostream>
#include "include/field.hpp"
#include "include/useful.hpp"

int main(int argc, char *argv[]) {
	Field test(10, 15);

	test.print();	
	test.testMoveAgent();
	test.print();

	return 0;
}
