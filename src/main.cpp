#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>
#include <typeinfo>
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
		beam_search.move(&field, MINE_ATTR);
		//astar.move(&field, MINE_ATTR);
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
int main(int argc, char* argv[]){
	
	if(argc <= 26){
		printf("\nparameters error\n");
		std::exit(0);
	}
	
	//---------------------- Node ----------------------
	move_weight = std::stof(argv[1]);
	state_weight = std::stof(argv[2]);
	heuristic_weight = std::stof(argv[3]);
	value_weight = std::stof(argv[4]);
	is_on_decided_route_weight = std::stof(argv[5]);
	is_on_mine_panel_weight = std::stof(argv[6]);
	is_on_enemy_panel_weight = std::stof(argv[7]);
	adjacent_agent_weight = std::stof(argv[8]);
	average_distance_weght = std::stof(argv[9]);

	//---------------------- Goal ----------------------
	occpancy_weight = std::stof(argv[10]);
	is_on_decided_weight = std::stof(argv[11]);
	is_angle_weight = std::stof(argv[12]);
	is_side_weight = std::stof(argv[13]);
	is_inside_closed_weight = std::stof(argv[14]);
	
	//---------------------- Branching ----------------------
	min_agent_distance = std::stof(argv[15]);
	min_goal_distance = std::stof(argv[16]);
	max_move = std::stof(argv[17]);
	min_move_cost = std::stof(argv[18]);
	min_value = std::stof(argv[19]);

	//---------------------- Score ----------------------
	score_weight = std::stof(argv[20]);
	goal_weight = std::stof(argv[21]);
	cost_weight = std::stof(argv[22]);

	//---------------------- Another ----------------------
	min_open_list_value = std::stof(argv[23]);
	greedy_count = std::stof(argv[24]);
	search_count = std::stof(argv[25]);

	//パラメータの表示
	std::cerr << "move_weight:" << move_weight << std::endl;
	std::cerr << "state_weight:" << state_weight << std::endl;
	std::cerr << "heuristic_weight:" << heuristic_weight << std::endl;
	std::cerr << "value_weight:" << value_weight << std::endl;
	std::cerr << "is_on_decided_route_weight:" << is_on_decided_route_weight << std::endl;
	std::cerr << "is_on_mine_panel_weight:" << is_on_mine_panel_weight << std::endl;
	std::cerr << "is_on_enemy_panel_weight:" << is_on_enemy_panel_weight << std::endl;
	std::cerr << "adjacent_agent_weight:" << adjacent_agent_weight << std::endl;
	std::cerr << "average_distance_weght:" << average_distance_weght << std::endl;

	std::cerr << "occpancy_weight" << occpancy_weight << std::endl;
	std::cerr << "is_on_decided_weight" << is_on_decided_weight << std::endl;
	std::cerr << "is_angle_weight" << is_angle_weight << std::endl;
	std::cerr << "is_side_weight" << is_side_weight << std::endl;
	std::cerr << "is_inside_closed_weight" << is_inside_closed_weight << std::endl;

	std::cerr << "min_agent_distance:" << min_agent_distance << std::endl;
	std::cerr << "min_goal_distance:" << min_goal_distance << std::endl;
	std::cerr << "max_move:" << max_move << std::endl;
	std::cerr << "min_move_cost:" << min_move_cost << std::endl;
	std::cerr << "min_value:" << min_value << std::endl;

	std::cerr << "score_weight:" << score_weight << std::endl;
	std::cerr << "goal_weight:" << goal_weight << std::endl;
	std::cerr << "cost_weight:" << cost_weight << std::endl;
		
	field.init();
	astar.init(&field);
	while(true){
		astar.move(&field, MINE_ATTR);
		//astar.move(&field, ENEMY_ATTR);
		
		//beam_search.move(&field, MINE_ATTR);
		//beam_search.move(&field, ENEMY_ATTR);
		
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

    //std::cerr << field.agents.size() << std::endl;

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
          
          std::cerr << "x:" << dx << " " << "y:" << dy << std::endl;

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
    std::cerr << value(actions) << std::endl;
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
