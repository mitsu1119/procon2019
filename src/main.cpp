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

/*
int main(int argc, char *argv[]) {
	field.init();

	DisplayWrapper* framework = new Display();
	framework->setField(&field);
	framework->setInstance(framework);
	framework->start(argc, argv);
	delete framework;

	return 0;	
}
*/

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

int main(int argc, char* argv[]){
	if(argc <= 26){
		printf("\nparameters error\n");
		std::exit(0);
	}

	//---------------------- Node ----------------------
	double_t _move_weight = std::stof(argv[1]);
	double_t _state_weight = std::stof(argv[2]);
	double_t _heuristic_weight = std::stof(argv[3]);
	double_t _value_weight = std::stof(argv[4]);
	double_t _is_on_decided_route_weight = std::stof(argv[5]);
	double_t _is_on_mine_panel_weight = std::stof(argv[6]);
	double_t _is_on_enemy_panel_weight = std::stof(argv[7]);
	double_t _adjacent_agent_weight = std::stof(argv[8]);
	double_t _average_distance_weght = std::stof(argv[9]);

	//---------------------- Another ----------------------
	double_t _min_open_list_value = std::stof(argv[23]);
	double_t _greedy_count = std::stof(argv[24]);
	double_t _search_count = std::stof(argv[25]);

	//---------------------- Goal ----------------------
	double_t _occpancy_weight = std::stof(argv[10]);
	double_t _is_on_decided_weight = std::stof(argv[11]);
	double_t _is_angle_weight = std::stof(argv[12]);
	double_t _is_side_weight = std::stof(argv[13]);
	double_t _is_inside_closed_weight = std::stof(argv[14]);
	
	//---------------------- Branching ----------------------
	double_t _min_agent_distance = std::stof(argv[15]);
	double_t _min_goal_distance = std::stof(argv[16]);
	double_t _max_move = std::stof(argv[17]);
	double_t _min_move_cost = std::stof(argv[18]);
	double_t _min_value = std::stof(argv[19]);

	//---------------------- Score ----------------------
	double_t _score_weight = std::stof(argv[20]);
	double_t _goal_weight = std::stof(argv[21]);
	double_t _cost_weight = std::stof(argv[22]);

	astar.setParams(_move_weight, _state_weight, _heuristic_weight, _value_weight, _is_on_decided_route_weight, _is_on_mine_panel_weight, _is_on_enemy_panel_weight, _adjacent_agent_weight, _average_distance_weght, _min_open_list_value, _greedy_count, _search_count, _occpancy_weight, _is_on_decided_weight, _is_angle_weight, _is_side_weight, _is_inside_closed_weight, _min_agent_distance, _min_goal_distance, _max_move, _min_move_cost, _min_value, _score_weight, _goal_weight, _cost_weight);

	/*
	//パラメータの表示
	std::cerr << "---------------------- Params ----------------------"  << std::endl;
	//---------------------- Node ----------------------
	std::cerr << "move_weight:" << _move_weight << std::endl;
	std::cerr << "state_weight:" << _state_weight << std::endl;
	std::cerr << "heuristic_weight:" << _heuristic_weight << std::endl;
	std::cerr << "value_weight:" << _value_weight << std::endl;
	std::cerr << "is_on_decided_route_weight:" << _is_on_decided_route_weight << std::endl;
	std::cerr << "is_on_mine_panel_weight:" << _is_on_mine_panel_weight << std::endl;
	std::cerr << "is_on_enemy_panel_weight:" << _is_on_enemy_panel_weight << std::endl;
	std::cerr << "adjacent_agent_weight:" << _adjacent_agent_weight << std::endl;
	std::cerr << "average_distance_weght:" << _average_distance_weght << std::endl;

	//---------------------- Another ----------------------
	std::cerr << "min_open_list_value" << _min_open_list_value << std::endl;
	std::cerr << "greedy_count" << _greedy_count << std::endl;
	std::cerr << "search_count" << _search_count << std::endl;

	//---------------------- Goal ----------------------
	std::cerr << "occpancy_weight" << _occpancy_weight << std::endl;
	std::cerr << "is_on_decided_weight" << _is_on_decided_weight << std::endl;
	std::cerr << "is_angle_weight" << _is_angle_weight << std::endl;
	std::cerr << "is_side_weight" << _is_side_weight << std::endl;
	std::cerr << "is_inside_closed_weight" << _is_inside_closed_weight << std::endl;

	//---------------------- Branching ----------------------
	std::cerr << "min_agent_distance:" << _min_agent_distance << std::endl;
	std::cerr << "min_goal_distance:" << _min_goal_distance << std::endl;
	std::cerr << "max_move:" << _max_move << std::endl;
	std::cerr << "min_move_cost:" << _min_move_cost << std::endl;
	std::cerr << "min_value:" << _min_value << std::endl;

	//---------------------- Score ----------------------
	std::cerr << "score_weight:" << _score_weight << std::endl;
	std::cerr << "goal_weight:" << _goal_weight << std::endl;
	std::cerr << "cost_weight:" << _cost_weight << std::endl;
	*/
	
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
