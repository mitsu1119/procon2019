#include <iostream>
#include <random>
#include <chrono>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

void selfDirectedGame();

int main(int argc, char *argv[]) {
	
	Field field(16, 16);

	DisplayWrapper* framework = new Display();
	
	framework->setField(&field);
	framework->setInstance(framework);
	framework->start(argc, argv);
	delete framework;
	
	return 0;
}

void selfDirectedGame(){
	//Random random;
	Greedy greedy;
	BeamSearch breem_search;
	Astar astar;
	
	Field field(16, 16);
	
	while(true){
		
		greedy.move(&field, MINE_ATTR);
		greedy.move(&field, ENEMY_ATTR);
		
		field.applyNextAgents();
		field.print();
		
		if(field.checkEnd()){
			field.judgeWinner();
			std::this_thread::sleep_for(std::chrono::minutes(1));
			std::exit(0);
		}
	}
}
