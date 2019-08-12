#include "ai.hpp"

AI::AI(){
}

AI::~AI(){
}

Astar::Astar(){	
}

Astar::~Astar(){
}

void Astar::move(Field *field){
	;
}

RandomMine::RandomMine(){
}

RandomMine::~RandomMine(){
}

void RandomMine::move(Field *field){
	std::random_device rnd;  
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<> rand(0, 8);
	Direction buf;
	for(auto &i: field->agents) {
		if(i.getAttr()==MINE_ATTR){
RE_CONSIDER:
			buf = (Direction)rand(mt);			
			if(field->canMove(i, buf))
				i.move(buf);
			else
				goto RE_CONSIDER;
		}
	}
}

RandomEnemy::RandomEnemy(){
}

RandomEnemy::~RandomEnemy(){
}

void RandomEnemy::move(Field *field){
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<> rand(0, 8);
	Direction buf;
	for(auto &i: field->agents) {
		if(i.getAttr()==ENEMY_ATTR){
RE_CONSIDER:
			buf = (Direction)rand(mt);
			if(field->canMove(i, buf))
				i.move(buf);
			else
				goto RE_CONSIDER;
		}
	}
}
