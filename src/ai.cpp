#include "ai.hpp"

//----------------AI--------------
AI::AI(){
}

AI::~AI(){
}

//----------------Node--------------
Node::Node(){
}

Node::~Node(){
}

const double Node::getScore() const{
	return this->moveCost + this->stateCost + this->heuristic;
}

//----------------A*algorithm--------------
AstarMine::AstarMine(){
}

AstarMine::~AstarMine(){
}

void AstarMine::init(const Field field){
	
}

void AstarMine::greedyMove(Field& field, const uint_fast32_t agent){
	
}

void AstarMine::decidedMove(Field& field, const uint_fast32_t agent){
	
}

const uint_fast32_t AstarMine::heuristicCost(std::pair<uint_fast32_t, uint_fast32_t> coord, std::pair<uint_fast32_t, uint_fast32_t> goal) const{
	const uint_fast32_t dx = std::abs((int_fast32_t)(goal.first - coord.first));
	const uint_fast32_t dy = std::abs((int_fast32_t)(goal.second - coord.second));
	const double distance = std::sqrt(dx * dy);
	return distance;
}

void AstarMine::sortSearchTargetList(const Field field, const uint_fast32_t agent){
	
}
	
const double AstarMine::estimeteMoveCost(Field& field, const uint_fast32_t agent) const{
	
}

void AstarMine::move(Field *field){
	
}

void AstarMine::print() const{
	
}

//----------------Random--------------
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
		if(i.getAttr() == MINE_ATTR){
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
		if(i.getAttr() == ENEMY_ATTR){
RE_CONSIDER:
			buf = (Direction)rand(mt);
			if(field->canMove(i, buf))
				i.move(buf);
			else
				goto RE_CONSIDER;
		}
	}
}
