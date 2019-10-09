#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <unistd.h>
#include <typeinfo>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

using namespace picojson;

time_t manage_time; // ゲーム管理用の時間

//Field field(16, 16);
Field field;

Greedy greedy;
BeamSearch beam_search;
Astar astar_mine;
Astar astar_enemy;

void move2json();
void time_process();
std::string getType(int nx, int ny, int dx, int dy);

/*
int main(int argc, char *argv[]) {
  manage_time = time(NULL); // とりあえず開始時の時刻を入れとく

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
  manage_time = time(NULL); // とりあえず開始時の時刻を入れとく
	
	field.init();
	astar_mine.init(&field);
	astar_enemy.init(&field);
	
	while(true){
		
		astar_mine.move(&field, MINE_ATTR);
		//astar_enemy.move(&field, ENEMY_ATTR);
		
		//beam_search.move(&field, MINE_ATTR);
		//beam_search.move(&field, ENEMY_ATTR);
		
		//greedy.move(&field, MINE_ATTR);
		greedy.move(&field, ENEMY_ATTR);

    move2json();
    system("python ../../post.py");

    time_process(); // 時間処理(仮) 
		
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
	//manage_time = time(NULL); // とりあえず開始時刻を設定

	field.init();

	if(argc < 26){
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
	double_t _greedy_count = std::stof(argv[10]);
	double_t _search_count = std::stof(argv[11]);
	double_t _min_open_list_value = std::stof(argv[12]);

	//---------------------- Goal ----------------------
	double_t _occpancy_weight = std::stof(argv[13]);
	double_t _is_on_decided_weight = std::stof(argv[14]);
	double_t _is_angle_weight = std::stof(argv[15]);
	double_t _is_side_weight = std::stof(argv[16]);
	double_t _is_inside_closed_weight = std::stof(argv[17]);
	
	//---------------------- Branching ----------------------
	double_t _min_agent_distance = std::stof(argv[18]);
	double_t _min_goal_distance = std::stof(argv[19]);
	double_t _max_move = std::stof(argv[20]);
	double_t _min_move_cost = std::stof(argv[21]);
	double_t _min_value = std::stof(argv[22]);

	//---------------------- Score ----------------------
	double_t _score_weight = std::stof(argv[23]);
	double_t _goal_weight = std::stof(argv[24]);
	double_t _cost_weight = std::stof(argv[25]);

	astar_mine.setParams(_move_weight, _state_weight, _heuristic_weight, _value_weight, _is_on_decided_route_weight, _is_on_mine_panel_weight, _is_on_enemy_panel_weight, _adjacent_agent_weight, _average_distance_weght, _min_open_list_value, _greedy_count, _search_count, _occpancy_weight, _is_on_decided_weight, _is_angle_weight, _is_side_weight, _is_inside_closed_weight, _min_agent_distance, _min_goal_distance, _max_move, _min_move_cost, _min_value, _score_weight, _goal_weight, _cost_weight);

	astar_mine.init(&field);

	if(argc == 26)
		goto _SINGLE_PLAY;

	if(argc < 51){
		printf("\nparameters error\n");
		std::exit(0);
	}		

	//---------------------- Node ----------------------
	_move_weight = std::stof(argv[26]);
	_state_weight = std::stof(argv[27]);
	_heuristic_weight = std::stof(argv[28]);
	_value_weight = std::stof(argv[29]);
	_is_on_decided_route_weight = std::stof(argv[30]);
	_is_on_mine_panel_weight = std::stof(argv[31]);
	_is_on_enemy_panel_weight = std::stof(argv[32]);
	_adjacent_agent_weight = std::stof(argv[33]);
	_average_distance_weght = std::stof(argv[34]);

	//---------------------- Another ----------------------
	_greedy_count = std::stof(argv[35]);
	_search_count = std::stof(argv[36]);
	_min_open_list_value = std::stof(argv[37]);

	//---------------------- Goal ----------------------
	_occpancy_weight = std::stof(argv[38]);
	_is_on_decided_weight = std::stof(argv[39]);
	_is_angle_weight = std::stof(argv[40]);
	_is_side_weight = std::stof(argv[41]);
	_is_inside_closed_weight = std::stof(argv[42]);
	
	//---------------------- Branching ----------------------
	_min_agent_distance = std::stof(argv[43]);
	_min_goal_distance = std::stof(argv[44]);
	_max_move = std::stof(argv[45]);
	_min_move_cost = std::stof(argv[46]);
	_min_value = std::stof(argv[47]);

	//---------------------- Score ----------------------
	_score_weight = std::stof(argv[48]);
	_goal_weight = std::stof(argv[49]);
	_cost_weight = std::stof(argv[50]);

	astar_enemy.setParams(_move_weight, _state_weight, _heuristic_weight, _value_weight, _is_on_decided_route_weight, _is_on_mine_panel_weight, _is_on_enemy_panel_weight, _adjacent_agent_weight, _average_distance_weght, _min_open_list_value, _greedy_count, _search_count, _occpancy_weight, _is_on_decided_weight, _is_angle_weight, _is_side_weight, _is_inside_closed_weight, _min_agent_distance, _min_goal_distance, _max_move, _min_move_cost, _min_value, _score_weight, _goal_weight, _cost_weight);
	
 _SINGLE_PLAY:
	astar_enemy.init(&field);
	
	while(true){

		std::cerr << "---------- MINE ----------" << std::endl;
		//astar_mine.move(&field, MINE_ATTR);
		std::cerr << "---------- ENEMY ----------" << std::endl;
		astar_enemy.move(&field, ENEMY_ATTR);

		beam_search.move(&field, MINE_ATTR);
		//beam_search.move(&field, ENEMY_ATTR);
		
		//greedy.move(&field, MINE_ATTR);
		//greedy.move(&field, ENEMY_ATTR);

		move2json();
    char *command = "python ../../test.py";
    int req;
    req = system(command);
		
		field.applyNextAgents();
		field.print();
		if(field.checkEnd()){
			field.judgeWinner();
			break;
		}
	}
	return 0;
}

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

void time_process(){
  int turnMillis     = (int)(field.getTurnMillis()/1000);      // 作戦時間
  int intervalMillis = (int)(field.getIntervalMillis()/1000);  // 遷移時間

  manage_time += turnMillis; // 作戦時間を加算しておく

  std::cerr << "[*] turn: " << manage_time - time(NULL) << " seconds left" <<std::endl;
  while(time(NULL) < manage_time){
    if(manage_time - time(NULL) <= 5)
      std::cerr << "[*] turn: " << manage_time - time(NULL) << " seconds left" << std::endl;
    system("python ../../post.py");
    sleep(1);
  }
  
  manage_time += intervalMillis; // 遷移時間を加算
  
  std::cerr << "[*] interval: " << intervalMillis << " seconds left" << std::endl;
  sleep(intervalMillis); // 遷移時間はsleepで待機

  std::cerr << "----- done -----" << std::endl;

}
