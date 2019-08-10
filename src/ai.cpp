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

Random::Random(){
}

Random::~Random(){
}

void Random::move(Field *field){
 	Direction buf;
	for(auto &i: field->agents) {
RE_CONSIDER:
		buf = (Direction)this->random(DIRECTION_SIZE - 1);
		if(field->canMove(i, buf))
			i.move(buf);
		else
			goto RE_CONSIDER;
	}
}

