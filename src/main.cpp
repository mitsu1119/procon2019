#include <iostream>
#include <random>
#include <chrono>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

int main(int argc, char *argv[]) {
	
	Field field(16, 16);

	DisplayWrapper* framework = new Display();
	
	framework->setField(&field);
	framework->setInstance(framework);
	framework->start(argc, argv);
	delete framework;
	
	return 0;
}
