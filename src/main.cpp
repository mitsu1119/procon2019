#include <iostream>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

//Field field(16, 16);
Field field;

//Random random;
Greedy greedy;
BeamSearch beam_search;
Astar astar;

/*
int main(int argc, char *argv[]) {

	DisplayWrapper* framework = new Display();

	framework->setField(&field);
	framework->setInstance(framework);
	framework->start(argc, argv);
	delete framework;

	return 0;
}
*/

int main(int argc, char *argv[]) {
	astar.init(&field);

	while(true){

		//astar.move(&field, MINE_ATTR);
		greedy.move(&field, MINE_ATTR);
		greedy.move(&field, ENEMY_ATTR);

		field.applyNextAgents();
		field.print();

		if(field.checkEnd()){
			field.judgeWinner();
			//std::this_thread::sleep_for(std::chrono::minutes(1));
			std::exit(0);
		}
	}

	return 0;
}
