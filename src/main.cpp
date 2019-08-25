#include <iostream>
#include <random>
#include <chrono>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

#include <numeric>
#include <string>
#include <math.h>
#include <cstdlib>
#include <time.h>

int main(int argc, char *argv[]) {
  //Calcmap objCalc;
  //objCalc.generateRndnum();
  srand((unsigned) time(NULL));

	Field field(10, 10);
  

	DisplayWrapper* framework = new Display();
	
	framework->setField(&field);
	framework->setInstance(framework);
	framework->start(argc, argv);
	delete framework;
	
	return 0;
}
