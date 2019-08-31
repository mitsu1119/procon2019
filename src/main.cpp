#include <iostream>
#include <random>
#include <chrono>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

#include "picojson.h"

void selfPlay();

int main(int argc, char *argv[]) {

	//selfPlay();

	Field field(20, 20);
	DisplayWrapper* framework = new Display();
	framework->setField(&field);
	framework->setInstance(framework);
	framework->start(argc, argv);
	delete framework;

	return 0;
}

void selfPlay(){
	Field field(20, 20);

	Random random;
	Greedy greedy;
	BeamSearch breem_search;
	Astar astar;

	while(true){

		greedy.move(&field, MINE_ATTR);
		greedy.move(&field, ENEMY_ATTR);

		field.applyNextAgents();
		field.print();
		if(field.checkEnd()){
			field.judgeWinner();
			std::this_thread::sleep_for(std::chrono::minutes(1));
		}
	}
}
