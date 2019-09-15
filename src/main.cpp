#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "field.hpp"
#include "useful.hpp"
#include "disp.hpp"

using namespace picojson;

//Field field(16, 16);
Field field;

//Random random;
Greedy greedy;
BeamSearch beam_search;
Astar astar;

void move2json();
void call_request();

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

void move2json(){
    object actions;

    array datalist;
    //int turn = Field.init().agent_num;	
    double j;

    double dx,dy; // 移動先
    double x, y;  // 移動前

    // forで毎回id,dx,dyのデータを取ってくる必要あり
    for(int i = 0; i < 7; i++){
    	{


        	object id;
      		j = (double)i+1;  // agentID
          x = (double)field.agents.at(i).getX();     // 移動前 x
          y = (double)field.agents.at(i).getY();     // 移動前 y
          dx= (double)field.agents.at(i).getnextX(); // 移動先 x
          dy= (double)field.agents.at(i).getnextY(); // 移動先 y

          dx -= x; // xの差をとる
          dy -= y; // yの差をとる

          std::cout << "x:" << dx << " " << "y:" << dy << std::endl;

        	//id.insert(std::make_pair("data", value(data)));

		      id.insert(std::make_pair("agentID", value(j)));
		      id.insert(std::make_pair("dx", value(dx)));
		      id.insert(std::make_pair("dy", value(dy)));
		      id.insert(std::make_pair("type", value("move")));

		
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

void call_request(){
  //char *command = "python ../../test.py"
  //int req;

  //system(command);

  //std::cout << req << std::endl;
}
