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

void AstarMine::init(const Field& field){
	
}

void AstarMine::greedyMove(Field& field, const uint_fast32_t agent){

	//修正必須
	if(agent >= field.agents.size())
		return;
	int_fast32_t x, y, buf, max = -100;
	Direction dir;
	for(size_t i = 0; i < DIRECTION_SIZE - 1; i++){
		if(field.canMove(field.agents.at(agent), (Direction)i)){
			x = field.agents.at(agent).getX() + this->vec_x.at(i);
			y = field.agents.at(agent).getY() + this->vec_y.at(i);
			buf = field.at(x, y)->getValue();
			if(buf > max){
				max = buf;
				dir = (Direction)i;
			}
		}
	}
	if(max == -100){
		field.agents.at(agent).move(STOP);
		return;
	}
	if(field.canMove(field.agents.at(agent), dir))
		field.agents.at(agent).move(dir);
}

void AstarMine::decidedMove(Field& field, const uint_fast32_t agent){
	
}

/*
void AstarMine::setGoalCondidateList(const Field& field){
	this->goal_condidate_list.clear();
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
			if(field.at(j, i)->getValue() > this->average_score)
				this->goal_condidate_list.push_back(std::make_pair(j, i));
		}
	}
}

void AstarMine::sortSearchTargetList(Field field, const uint_fast32_t agent){
	if(agent >= field.agents.size())
		return;
	this->search_target_list.clear();
	int_fast32_t value, x, y;
	//	double heuristic;
	std::vector<std::pair<double, std::pair<uint_fast32_t, uint_fast32_t>>> condidate_list;
	std::for_each(this->goal_condidate_list.begin(), this->goal_condidate_list.end(), [&, this](auto coord){
			x = coord.first;
			y = coord.second;
			if(field.agents.at(agent).getX() == x && field.agents.at(agent).getY() == y)
				return;
			//			heuristic = this->heuristicCost(std::make_pair(field.agents.at(agent).getX(), field.agents.at(agent).getY()), std::make_pair(x, y));
			value = field.at(x, y)->getValue();
			condidate_list.push_back(std::make_pair(value, std::make_pair(x, y)));
		});
	std::sort(condidate_list.rbegin(), condidate_list.rend());
	for(size_t i = 0; i < this->search_depth; i++)
		if(i < condidate_list.size())
			this->search_target_list.push_back(condidate_list.at(i).second);
	
	std::for_each(this->search_target_list.begin(), this->search_target_list.end(), [&, this](auto& a){
			std::cout << "(" << a.first << "," << a.second << ")" << std::endl;
		});
}
*/

void AstarMine::setAverageScore(const Field& field){
	int val = 0, buf = 0;
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
			buf += field.at(j, i)->getValue();
			val++;
		}
	}
	this->average_score = buf / val;
}

void AstarMine::setSearchTargetList(Field field, const uint_fast32_t agent){
	std::vector<std::pair<double, std::pair<uint_fast32_t, uint_fast32_t>>> condidate_list;
	int_fast32_t value;
	if(agent >= field.agents.size())
		return;
	this->search_target_list.clear();
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
	 
			value = this->selectGoalCondidate(field, agent, std::make_pair(j, i));
			if(value)
				condidate_list.push_back(std::make_pair(value, std::make_pair(j, i)));
			
		}
	}
	std::sort(condidate_list.rbegin(), condidate_list.rend());
	for(size_t i = 0; i < this->search_depth; i++)
		if(i < condidate_list.size())
			this->search_target_list.push_back(condidate_list.at(i).second);

	
	std::for_each(this->search_target_list.begin(), this->search_target_list.end(), [&, this](auto& a){
			std::cout << "(" << a.first << "," << a.second << ")" << std::endl;
		});

	
}

int_fast32_t AstarMine::selectGoalCondidate(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> coord){
	int_fast32_t value;
	double heuristic;
	if(field.agents.at(agent).getX() == coord.first && field.agents.at(agent).getY() == coord.second)
		return -1;
	if(this->isDecidedRoute(field, agent, coord))
		return -1;
	heuristic = this->heuristicCost(std::make_pair(field.agents.at(agent).getX(), field.agents.at(agent).getY()), std::make_pair(coord.first, coord.second));
	value = field.at(coord.first, coord.second)->getValue();

	
	return value;
}

bool AstarMine::isDecidedRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> coord){
	if(this->decided_route.empty())
		return false;
	
	for(size_t i = 0; i < field.getWidth(); i++){
		if(this->decided_route.at(i).empty())
			continue;
		auto result = std::find(this->decided_route.at(i).begin(), this->decided_route.at(i).end(), coord);
		if(result != this->decided_route.at(i).end())
			return true;
	}
	
	return false;
}

const uint_fast32_t AstarMine::heuristicCost(std::pair<uint_fast32_t, uint_fast32_t> coord, std::pair<uint_fast32_t, uint_fast32_t> goal) const{
	const uint_fast32_t dx = std::abs((int_fast32_t)(goal.first - coord.first));
	const uint_fast32_t dy = std::abs((int_fast32_t)(goal.second - coord.second));
	const double distance = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
	return distance;
}

const double AstarMine::estimeteMoveCost(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> goal) const{
	
}

void AstarMine::move(Field *field){
	Field fields = static_cast<Field> (*field);
	for(size_t i = 0; i < fields.agents.size(); i++)
		this->greedyMove(fields, i);
	fields.applyNextAgents();		
	*field = fields;
	//this->setAverageScore(fields);
	//this->setSearchTargetList(fields, 0);
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
	std::uniform_int_distribution<> rand(0, DIRECTION_SIZE - 2);
	uint_fast32_t val = 0;
	Direction buf;
	for(auto &i: field->agents) {
		if(i.getAttr() == MINE_ATTR){
RE_CONSIDER:
			if(val > 100){
				i.move(STOP);
				return;
			}
			buf = (Direction)rand(mt);			
			if(field->canMove(i, buf))
				i.move(buf);
			else{
				val++;
				goto RE_CONSIDER;
			}
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
	std::uniform_int_distribution<> rand(0, DIRECTION_SIZE - 2);
	uint_fast32_t val = 0;
	Direction buf;
	for(auto &i: field->agents) {
		if(i.getAttr() == ENEMY_ATTR){
RE_CONSIDER:
			if(val > 100){
				i.move(STOP);
				return;
			}
			buf = (Direction)rand(mt);
			if(field->canMove(i, buf))
				i.move(buf);
			else{
				val++;
				goto RE_CONSIDER;
			}
		}
	}
}
