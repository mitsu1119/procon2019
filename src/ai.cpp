#include "ai.hpp"

//----------------AI--------------
AI::AI(){
}

AI::~AI(){
}

//----------------Greedy--------------

Greedy::Greedy(){	
}

Greedy::~Greedy(){	
}

void Greedy::mineMove(Field& field){
	this->decided_coord.clear();
	for(size_t i =0; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == MINE_ATTR)
			this->singleMove(field, i);
}

void Greedy::enemyMove(Field& field){
	this->decided_coord.clear();
	for(size_t i =0; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == ENEMY_ATTR)
			this->singleMove(field, i);
}

void Greedy::singleMove(Field& field, const uint_fast32_t agent){
	int_fast32_t max = 0, value = -1000;
	Direction direction = STOP;
	for(size_t i = 0; i < DIRECTION_SIZE - 2; i++){
		auto result = std::find(this->decided_coord.begin(), this->decided_coord.end(), std::make_pair(field.agents.at(agent).getX() + this->vec_x.at(i), field.agents.at(agent).getY() + this->vec_y.at(i)));
		if(result != this->decided_coord.end())
			continue;
		if(!field.canMove(field.agents.at(agent), (Direction)i))
			continue;
		value = this->nextScore(field, agent, (Direction)i);
		if(value > max){
			max = value;
			direction = (Direction)i;
		}
	}
	field.agents.at(agent).move(direction);
	this->decided_coord.push_back(std::make_pair(field.agents.at(agent).getX() + this->vec_x.at(direction), field.agents.at(agent).getY() + this->vec_y.at(direction)));
}

int_fast32_t Greedy::nextScore(Field field, const uint_fast32_t agent, Direction direction) const{
	if(agent >= field.agents.size())
		return -1;
	
	std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& a){
			a.move(STOP);
		});
	if(field.canMove(field.agents.at(agent), direction))
		field.agents.at(agent).move(direction);
	field.applyNextAgents();
	
	if(field.agents.at(agent).getAttr() == MINE_ATTR)
		return field.calcMinepanelScore() - field.calcEnemypanelScore();
		//return field.calcScore(MINE_ATTR) - field.calcScore(ENEMY_ATTR);
	else
		return field.calcEnemypanelScore() - field.calcMinepanelScore();
		//return field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	
	return -1;
}

void Greedy::move(Field *field, uint_fast32_t attr){
	Field obj = static_cast<Field> (*field);
	if(attr == MINE_ATTR)
		this->mineMove(obj);
	else
		this->enemyMove(obj);
	*field = obj;
}

//----------------Node--------------
Node::Node(){
}

Node::~Node(){
	delete parent;
}

const double Node::getScore() const{
	return this->moveCost + this->stateCost + this->heuristic;
}

//----------------A*algorithm--------------
Astar::Astar(){
}

Astar::~Astar(){
}

void Astar::init(const Field& field){
	
}

void Astar::decidedMove(Field& field, const uint_fast32_t agent){
	
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


void Astar::setAverageScore(const Field& field){
	int val = 0, buf = 0;
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
			buf += field.at(j, i)->getValue();
			val++;
		}
	}
	this->average_score = buf / val;
}

void Astar::setSearchTargetList(Field field, const uint_fast32_t agent){
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

int_fast32_t Astar::selectGoalCondidate(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> coord){
	int_fast32_t value;
	double heuristic;
	if(agent >= field.agents.size());
		return -1;
	if(field.agents.at(agent).getX() == coord.first && field.agents.at(agent).getY() == coord.second)
		return -1;
	if(this->isDecidedRoute(field, agent, coord))
		return -1;
	heuristic = this->heuristicCost(std::make_pair(field.agents.at(agent).getX(), field.agents.at(agent).getY()), std::make_pair(coord.first, coord.second));
	value = field.at(coord.first, coord.second)->getValue();

	
	return value;
}

bool Astar::isDecidedRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> coord){
	if(agent >= field.agents.size())
		return false;

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

const uint_fast32_t Astar::heuristicCost(std::pair<uint_fast32_t, uint_fast32_t> coord, std::pair<uint_fast32_t, uint_fast32_t> goal) const{
	const uint_fast32_t dx = std::abs((int_fast32_t)(goal.first - coord.first));
	const uint_fast32_t dy = std::abs((int_fast32_t)(goal.second - coord.second));
	const double distance = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
	return distance;
}

const double Astar::estimeteMoveCost(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> goal) const{
	
}

void Astar::move(Field *field, uint_fast32_t attr){
	Field obj = static_cast<Field> (*field);
	//this->setAverageScore(fields);
	//this->setSearchTargetList(fields, 0);
}

void Astar::print() const{
	
}

//----------------Random--------------
Random::Random(){
}

Random::~Random(){
}

void Random::mineMove(Field& field){
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<> rand(0, DIRECTION_SIZE - 3);
	uint_fast32_t val = 0;
	Direction buf;
	for(auto &i: field.agents) {
		if(i.getAttr() == MINE_ATTR){
RE_CONSIDER:
			if(val > 100){
				i.move(STOP);
				return;
			}
			buf = (Direction)rand(mt);			
			if(field.canMove(i, buf))
				i.move(buf);
			else{
				val++;
				goto RE_CONSIDER;
			}
		}
	}
}

void Random::enemyMove(Field& field){
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<> rand(0, DIRECTION_SIZE - 3);
	uint_fast32_t val = 0;
	Direction buf;
	for(auto &i: field.agents) {
		if(i.getAttr() == ENEMY_ATTR){
RE_CONSIDER:
			if(val > 100){
				i.move(STOP);
				return;
			}
			buf = (Direction)rand(mt);			
			if(field.canMove(i, buf))
				i.move(buf);
			else{
				val++;
				goto RE_CONSIDER;
			}
		}
	}
}

void Random::move(Field *field, uint_fast32_t attr){
	Field obj = static_cast<Field> (*field);
	if(attr == MINE_ATTR)
		this->mineMove(obj);
	else
		this->enemyMove(obj);
	*field = obj;
}

