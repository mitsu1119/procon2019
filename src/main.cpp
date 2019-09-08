#include <iostream>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

Field field(12, 16);
//Field field;

Greedy greedy;
BeamSearch beam_search;
Astar astar;

int main(int argc, char *argv[]) {

	//field.init();

	DisplayWrapper* framework = new Display();

	framework->setField(&field);
	framework->setInstance(framework);
	framework->start(argc, argv);
	delete framework;

	return 0;
	
}

/*
int main(int argc, char *argv[]){
	
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
*/


/*
static double_t move_weight;
static double_t state_weight;
static double_t heuristic_weight;
static double_t value_weight;
static double_t is_on_decided_route_weight;
static double_t is_on_mine_panel_weight;
static double greedy_count;
static double occpancy_weight;
static double is_on_decided_weight;
static uint_fast32_t max_mine_distance;
static uint_fast32_t min_mine_distance;
static uint_fast32_t min_agent_distance;
static uint_fast32_t min_goal_distance;
static uint_fast32_t max_move_cost;
static uint_fast32_t min_value;
static uint_fast32_t min_move_cost;
static uint_fast32_t search_count;
*/

/*
int main(int argc, double_t argv[]){
	if(argc < 18){
		printf("\nparameters error\n");
		return 0;
	}
	//---------------- Node ----------------
	//double
	move_weight                = argv[1];
	state_weight               = argv[2];
	heuristic_weight           = argv[3];
	value_weight               = argv[4];
	is_on_decided_route_weight = argv[5];
	is_on_mine_panel_weight    = argv[6];
	//---------------- Goal ----------------
	//double
	greedy_count               = argv[7];
	occpancy_weight            = argv[8];
	is_on_decided_weight       = argv[9];
	//---------------- Branching ----------------
	//uint_fast32_t
	max_mine_distance          = argv[10];
	min_mine_distance          = argv[11];
	min_agent_distance         = argv[12];
	min_goal_distance          = argv[13];
	max_move_cost              = argv[14];
	min_value                  = argv[15];
	min_move_cost              = argv[16];
	search_count               = argv[17];
	
	astar.init(&field);
	while(true){
		//astar.move(&field, MINE_ATTR);
		//astar.move(&field, ENEMY_ATTR);
		
		//beam_search.move(&field, MINE_ATTR);
		//beam_search.move(&field, ENEMY_ATTR);
		
		greedy.move(&field, MINE_ATTR);
		greedy.move(&field, ENEMY_ATTR);
		field.applyNextAgents();
		field.print();
		if(field.checkEnd()){
			field.judgeWinner();
			std::exit(0);
		}
	}
	return 0;
}
*/

