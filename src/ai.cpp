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

int_fast32_t Greedy::nextScore(Field field, const uint_fast32_t agent, const Direction direction) const{
	if(agent >= field.agents.size())
		return -1;
	if(field.canMove(field.agents.at(agent), direction))
		field.agents.at(agent).move(direction);
	field.applyNextAgents();
	if(field.agents.at(agent).getAttr() == MINE_ATTR)
		return field.calcScore(MINE_ATTR) - field.calcScore(ENEMY_ATTR);
	else
		return field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	return -1;
}

void Greedy::move(Field *field, const uint_fast32_t attr){
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

void Astar::decidedMove(Field& field, const uint_fast32_t agent){
	
}

void Astar::setAverageScore(const Field& field){
	int count = 0, sum = 0;
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
			sum += field.at(j, i)->getValue();
			count++;
		}
	}
	this->average_score = sum / count;
}

void Astar::setSearchTarget(Field field, const uint_fast32_t agent){
	std::vector<std::pair<double, std::pair<uint_fast32_t, uint_fast32_t>>> condidate;
	if(agent >= field.agents.size())
		return;
	this->search_target.clear();
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
			if(this->goalEvaluation(field, agent, std::make_pair(j, i)))
				condidate.push_back(std::make_pair(this->goalEvaluation(field, agent, std::make_pair(j, i)), std::make_pair(j, i)));
		}
	}
	std::sort(condidate.rbegin(), condidate.rend());
	for(size_t i = 0; i < this->search_depth; i++){
		if(i >= condidate.size())
			break;
		this->search_target.push_back(condidate.at(i).second);
	}
}

int_fast32_t Astar::goalEvaluation(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal){
	int_fast32_t value;
	double heuristic;
	if(this->expectTarget(field, agent, goal))
		return false;
	return field.at(goal.first, goal.second)->getValue() + this->occupancyRate(field, agent, goal) * 2;
}

const bool Astar::expectTarget(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	if(agent >= field.agents.size())
		return true;
	if(field.agents.at(agent).getX() == coord.first && field.agents.at(agent).getY() == coord.second)
		return true;
	if(field.at(coord.first, coord.second)->getValue() < this->average_score)
		return true;		
	if(this->isOnDecidedRoute(field, agent, coord))
		return true;
	if(this->whosePanel(field, agent, coord) == MINE_ATTR)
		return true;
	if(this->anotherDistance(field, agent, coord))
		return true;
	return false;
}

const bool Astar::isOnDecidedRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	for(size_t i = 0; i < this->decided_route.size(); i++){
		if(this->decided_route.at(i).size() == 0)
			continue;
		if(field.agents.at(agent).getAttr() != field.agents.at(i).getAttr())
			continue;
		auto result = std::find(this->decided_route.at(i).begin(), this->decided_route.at(i).end(), coord);
		if(result != this->decided_route.at(i).end())
			return true;
	}
	return false;
}

const bool Astar::anotherDistance(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	double mine_distance, another_distance;
	mine_distance = this->heuristic(std::make_pair(field.agents.at(agent).getX(), field.agents.at(agent).getY()), coord);
	if(mine_distance < 3 || mine_distance > 7)
		return true;
	for(size_t i = 0; i < field.agents.size(); i++){
		if(agent == i)
			continue;
		if(field.agents.at(agent).getAttr() != field.agents.at(i).getAttr())
			continue;
		another_distance = this->heuristic(std::make_pair(field.agents.at(i).getX(), field.agents.at(i).getY()), coord);		
		if(another_distance < mine_distance && another_distance <= 2)
			return true;
	}
	return false;
}

const uint_fast32_t Astar::whosePanel(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t attr = MINE_ATTR;
	if(field.at(coord.first, coord.second)->getAttr() == PURE_ATTR)
	  attr = PURE_ATTR;
	if(field.agents.at(agent).getAttr() == MINE_ATTR && field.at(coord.first, coord.second)->getAttr() == ENEMY_ATTR)
		attr = ENEMY_ATTR;
	if(field.agents.at(agent).getAttr() == ENEMY_ATTR && field.at(coord.first, coord.second)->getAttr() == MINE_ATTR)
		attr = ENEMY_ATTR;
	return attr;
}

const uint_fast32_t Astar::occupancyRate(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t count = 0;
	for(size_t i = 0; i < DIRECTION_SIZE - 1; i++){
		if(this->whosePanel(field, agent, std::make_pair(coord.first + this->vec_x.at(i), coord.second + this->vec_y.at(i))) == ENEMY_ATTR)
			count++;
	}
	return count;
}

const uint_fast32_t Astar::heuristic(std::pair<uint_fast32_t, uint_fast32_t> coord, std::pair<uint_fast32_t, uint_fast32_t> goal) const{
	const uint_fast32_t dx = std::abs((int_fast32_t)(goal.first - coord.first));
	const uint_fast32_t dy = std::abs((int_fast32_t)(goal.second - coord.second));
	const double distance = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
	return distance;
}

const double Astar::search(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> goal) const{
	
}

void Astar::init(const Field& field){
	this->setAverageScore(field);	
}

void Astar::mineMove(Field& field){
	
}

void Astar::enemyMove(Field& field){
	
}

void Astar::move(Field *field, const uint_fast32_t attr){
	Field obj = static_cast<Field> (*field);
	this->init(obj);
	this->setSearchTarget(obj, 0);
	this->print();
}

void Astar::print() const{
	
	std::for_each(this->search_target.begin(), this->search_target.end(), [&, this](auto& coord){
			std::cout<< "(" << coord.first << "," << coord.second << ")" << std::endl;
		});
	
	const uint_fast32_t cell_size  = 30;
	const uint_fast32_t point_size = 15;
	const uint_fast32_t half = cell_size / 2;
	glPointSize(point_size);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	std::for_each(this->search_target.begin(), this->search_target.end(), [&, this](auto& coord){
			glVertex2i(half+cell_size * coord.first, half + cell_size * coord.second);
		});
	glEnd();
	glFlush();
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

void Random::move(Field *field, const uint_fast32_t attr){
	Field obj = static_cast<Field> (*field);
	if(attr == MINE_ATTR)
		this->mineMove(obj);
	else
		this->enemyMove(obj);
	*field = obj;
}

