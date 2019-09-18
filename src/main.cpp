#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

using namespace picojson;

//Field field(16, 16);
Field field;

Greedy greedy;
BeamSearch beam_search;
Astar astar;

void move2json();
std::string getType(int nx, int ny, int dx, int dy);

int main(int argc, char *argv[]) {
	field.init();

	DisplayWrapper* framework = new Display();
	framework->setField(&field);
	framework->setInstance(framework);
	framework->start(argc, argv);
	delete framework;

	return 0;
	
}

/*
int main(int argc, char *argv[]){
	field.init();
	astar.init(&field);
	while(true){
		astar.move(&field, MINE_ATTR);
		//greedy.move(&field, MINE_ATTR);
		greedy.move(&field, ENEMY_ATTR);
		field.applyNextAgents();
		field.print();
		if(field.checkEnd()){
			field.judgeWinner();
			break;
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
			break;
		}
	}
	return 0;
}
*/

/*
int main(int argc, char *argv[]) {
	astar.init(&field);
	field.init();

	while(true){
		
		//astar.move(&field, MINE_ATTR);
		greedy.move(&field, MINE_ATTR);
		greedy.move(&field, ENEMY_ATTR);
		
		// Debug ---------------------------------------
		move2json();

    // Debug ---------------------
    char *command = "python ../../test.py";
    int req;

    req = system(command);

    // ----------------------------

		field.applyNextAgents();

		field.print();

		if(field.checkEnd()){
			field.judgeWinner();
			//move2json();
			//std::this_thread::sleep_for(std::chrono::minutes(1));
			break;
		}
	}
	
	
	
	return 0;
}
*/

void move2json(){
    object actions;

    array datalist;
    //int turn = Field.init().agent_num;	
    double j;
    double ID;

    double dx,dy; // 移動先
    double nx,ny;
    double x, y;  // 移動前

    std::string type;

    //std::cout << field.agents.size() << std::endl;

    // forで毎回id,dx,dyのデータを取ってくる必要あり
    for(int i = 0; i < field.agents.size()/2; i++){
    	{


        	object id;
          ID= (double)field.agents.at(i).getID();    // agentID
      		//j = (double)i+1;  // agentID
          x = (double)field.agents.at(i).getX();     // 移動前 x
          y = (double)field.agents.at(i).getY();     // 移動前 y
          nx= (double)field.agents.at(i).getnextX(); // 移動先 x
          ny= (double)field.agents.at(i).getnextY(); // 移動先 y

          dx = nx - x; // xの差をとる
          dy = ny - y; // yの差をとる
          
          std::cout << "x:" << dx << " " << "y:" << dy << std::endl;

        	//id.insert(std::make_pair("data", value(data)));
          
          // Debug
          type = getType((int)nx, (int)ny , (int)dx, (int)dy);

		      id.insert(std::make_pair("agentID", value(ID)));
		      id.insert(std::make_pair("dx", value(dx)));
		      id.insert(std::make_pair("dy", value(dy)));
		      id.insert(std::make_pair("type", value(type)));

		
        	datalist.push_back(value(id));
		

    	}
	
    }	


    // データまとめ   
    actions.insert(std::make_pair("actions", value(datalist)));

    // データ表示
    std::cout << value(actions) << std::endl;
    // データ書き込み
    std::ofstream outputfile("../../action.json");

    outputfile << value(actions);

    outputfile.close();
}

std::string getType(int nx, int ny, int dx, int dy){
    uint_fast32_t attr;
    if((dx == 0) && (dy == 0)){
      return "stay";
    }else{
      attr = field.at(nx,ny)->getAttr();
      if(attr == ENEMY_ATTR)
        return "remove";
      else
        return "move";
    }
}
