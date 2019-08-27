#include "ai.hpp"

//----------------AI--------------
AI::AI(){
}

AI::~AI(){
}

//----------------Random--------------
Random::Random(){
	this->random = XorOshiro128p(time(NULL));
}

Random::~Random(){
}

void Random::mineMove(Field& field){
	uint_fast32_t count = 0;
	Direction direction;
	for(auto &i: field.agents) {
		if(i.getAttr() == MINE_ATTR){
RE_CONSIDER:
			if(count++ > 10)
				break;
			direction = (Direction)(this->random(DIRECTION_SIZE - 3));
			if(field.canMove(i, direction))
				i.move(direction);
			else
				goto RE_CONSIDER;
		}
	}
}

void Random::enemyMove(Field& field){
	uint_fast32_t count = 0;
	Direction direction;
	for(auto &i: field.agents) {
		if(i.getAttr() == ENEMY_ATTR){
RE_CONSIDER:
			if(count++ > 10)
				break;
			direction = (Direction)(this->random(DIRECTION_SIZE - 3));
			if(field.canMove(i, direction))
				i.move(direction);
			else
				goto RE_CONSIDER;
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

//----------------Greedy--------------

Greedy::Greedy(){
	this->random = XorOshiro128p(time(NULL));
}

Greedy::~Greedy(){	
}

void Greedy::mineMove(Field& field){
	current_score = field.calcScore(MINE_ATTR)  - field.calcScore(ENEMY_ATTR);
	this->decided_coord.clear();
	for(size_t i =0; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == MINE_ATTR)
			this->singleMove(field, i);
}

void Greedy::enemyMove(Field& field){
	current_score = field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	this->decided_coord.clear();
	for(size_t i =0; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == ENEMY_ATTR)
			this->singleMove(field, i);
}

void Greedy::singleMove(Field& field, const uint_fast32_t agent){
	int_fast32_t max_score   = - INT_MAX;
	int_fast32_t next_score  = - INT_MAX;
	//uint_fast32_t x          = field.agents.at(agent).getX();
	//uint_fast32_t y          = field.agents.at(agent).getY();
	Direction direction      = STOP;
	
	for(size_t i = 0; i < DIRECTION_SIZE - 2; i++){
		/*
		auto result = std::find(this->decided_coord.begin(), this->decided_coord.end(), std::make_pair(x + this->vec_x.at(i), y + this->vec_y.at(i)));
		if(result != this->decided_coord.end())
			continue;
		*/
		if(field.canMove(field.agents.at(agent), (Direction)i)){
			next_score = this->nextScore(field, agent, (Direction)i);
			if(next_score <= this->current_score)
				continue;
			if(next_score > max_score){
				max_score = next_score;
				direction = (Direction)i;
			}
		}
	}
	
	if(max_score == - INT_MAX)
		this->randomMove(field, agent, field.agents.at(agent).getX(), field.agents.at(agent).getY());
	else{
		field.agents.at(agent).move(direction);
		//this->decided_coord.push_back(std::make_pair(x + this->vec_x.at(direction), y + this->vec_y.at(direction)));
	}
}

void Greedy::randomMove(Field& field, const uint_fast32_t agent, const uint_fast32_t x, const uint_fast32_t y){
	Direction direction = STOP;
	uint_fast32_t count = 0;
	while(true){
		if(count++ > 10)
			break;
		direction = (Direction)(this->random(DIRECTION_SIZE - 3));
		/*
		auto result = std::find(this->decided_coord.begin(), this->decided_coord.end(), std::make_pair(x + this->vec_x.at(direction), y + this->vec_y.at(direction)));
		if(result != this->decided_coord.end())
			continue;
		*/
		if(field.at(x + this->vec_x.at(direction), y + this->vec_y.at(direction))->getValue() < -10)
			continue;
		if(field.canMove(field.agents.at(agent), direction)){
			field.agents.at(agent).move(direction);
			//this->decided_coord.push_back(std::make_pair(x + this->vec_x.at(direction), y + this->vec_y.at(direction)));
			return;
		}
	}
}

int_fast32_t Greedy::nextScore(Field field, const uint_fast32_t agent, const Direction direction) const{
	field.agents.at(agent).move(direction);
	field.applyNextAgents();
	if(field.agents.at(agent).getAttr() == MINE_ATTR)
		return field.calcScore(MINE_ATTR) - field.calcScore(ENEMY_ATTR);
	else
		return field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	return false;
}

void Greedy::move(Field *field, const uint_fast32_t attr){
	Field obj = static_cast<Field> (*field);
	if(attr == MINE_ATTR)
		this->mineMove(obj);
	else
		this->enemyMove(obj);
	*field = obj;
}

//----------------BeamSearch--------------
BeamSearch::BeamSearch(){
}

BeamSearch::~BeamSearch(){	
}

void BeamSearch::mineMove(Field& field){
	
}

void BeamSearch::enemyMove(Field& field){
	
}

void BeamSearch::move(Field *field, const uint_fast32_t attr){
	
}

//----------------Nod--------------
BreadthForceSearch::BreadthForceSearch(){	
}

BreadthForceSearch::~BreadthForceSearch(){
}


//----------------BreadthForceSearch--------------
void BreadthForceSearch::mineMove(Field& field){
}

void BreadthForceSearch::enemyMove(Field& field){
}

void BreadthForceSearch::move(Field *field, const uint_fast32_t attr){
	
}

//----------------Node--------------
Node::Node(){
	this->status = NONE;
	this->parent = nullptr;
}

Node::~Node(){
}

//----------------A*algorithm--------------
Astar::Astar(){
}

Astar::~Astar(){
}

void Astar::greedyMove(Field& field, const uint_fast32_t agent, const uint_fast32_t move_num){
	if(move_num > 4)
		return;
	this->greedy.enemyMove(field);
	
	/*
	for(size_t i = agent + 1; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == field.agents.at(agent).getAttr())
			this->greedy.singleMove(field, i);
	*/
}


void Astar::decidedMove(Field& field, const uint_fast32_t agent, std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>>& route){
	Direction direction;
	for(size_t i = 0; i < agent; i++){
		if(route.at(i).size() < 3)
			continue;
		direction = this->changeDirection(route.at(i).at(0), route.at(i).at(1));
		if(field.canMove(field.agents.at(i), direction))
			field.agents.at(i).move(direction);
	}
	
	field.applyNextAgents();
	
	for(size_t i = 0; i < agent; i++){
		if(route.at(i).empty())
			continue;
		if(field.agents.at(i).getX() != route.at(i).at(0).first || field.agents.at(i).getY() != route.at(i).at(0).second)
			route.at(i).erase(route.at(i).begin());
	}
}

void Astar::setAverageScore(const Field& field){
	int_fast32_t count = 0, sum = 0;
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
			sum += field.at(j, i)->getValue();
			count++;
		}
	}
	this->average_score = sum / count;
}

void Astar::setSearchTarget(Field& field, const uint_fast32_t agent){
	std::vector<std::pair<double, std::pair<uint_fast32_t, uint_fast32_t>>> condidate;
	double value;
	this->search_target.clear();	
	if(agent >= field.agents.size())
		return;
	this->search_target.clear();
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
			value = this->goalEvaluation(field, agent, std::make_pair(j, i));
			if(value)
				condidate.push_back(std::make_pair(value, std::make_pair(j, i)));
		}
	}
	if(condidate.empty())
		return;
	std::sort(condidate.rbegin(), condidate.rend());
	for(size_t i = 0; i < search_depth; i++){
		if(i >= condidate.size())
			break;
		this->search_target.push_back(condidate.at(i).second);
	}
}

const double Astar::goalEvaluation(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal){
	if(this->expectTarget(field, agent, goal))
		return false;
	return field.at(goal.first, goal.second)->getValue() + this->occupancyRate(field, agent, goal) * 1.5;
}

const uint_fast32_t Astar::occupancyRate(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t count = 0;
	for(size_t i = 0; i < DIRECTION_SIZE - 1; i++){
		if(this->whosePanel(field, agent, std::make_pair(coord.first + this->vec_x.at(i), coord.second + this->vec_y.at(i))) == ENEMY_ATTR)
			count++;
	}
	return count;
}

const uint_fast32_t Astar::isSideOrAngle(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord){
	if(coord.first == 0 && coord.second == 0 || coord.first == 0 && coord.second == field.getHeight() - 1 || coord.first == field.getWidth() - 1 && coord.second == 0 || coord.first == field.getWidth() - 1 && coord.second == field.getHeight() - 1)
		return 1;
	if(coord.first == 0 || coord.second == 0 || coord.first == field.getWidth() - 1 || coord.second == field.getHeight() - 1)
		return 2;
	return false;
}

const bool Astar::expectTarget(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord){
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
	if(this->anotherAgentDistance(field, agent, coord))
		return true;
	if(this->anotherGoalDistance(field, agent, coord))
		return true;
	return false;
}

const bool Astar::isOnDecidedRoute(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	if(this->decided_coord.empty())
		return false;
	auto result = std::find(this->decided_coord.begin(), this->decided_coord.end(), coord);
	if(result != this->decided_coord.end())
			return true;
	return false;
}

const bool Astar::anotherAgentDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t x, y;
	double mine_distance, another_distance;
	x = field.agents.at(agent).getX();
	y = field.agents.at(agent).getY();
	mine_distance = this->heuristic(std::make_pair(x, y), coord);
	if(mine_distance < 3 || mine_distance > 10)
		return true;
	for(size_t i = 0; i < field.agents.size(); i++){
		if(agent == i)
			continue;
		if(field.agents.at(agent).getAttr() != field.agents.at(i).getAttr())
			continue;
		another_distance = this->heuristic(std::make_pair(x, y), coord);
		if(another_distance < mine_distance && another_distance <= 3)
			return true;
	}
	return false;
}

const bool Astar::anotherGoalDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t distance;
	for(size_t i = 0; i < agent; i++){
		if(field.agents.at(agent).getAttr() == field.agents.at(i).getAttr()){
			distance = this->heuristic(coord, this->decided_goal.at(i));
			if(distance <= 3)
				return true;
		}
	}
	return false;
}

const uint_fast32_t Astar::whosePanel(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t attr = MINE_ATTR;
	uint_fast32_t	agent_attr  = field.at(coord.first, coord.second)->getAttr();
	uint_fast32_t	pannel_attr = field.at(coord.first, coord.second)->getAttr();
	if(pannel_attr == PURE_ATTR)
	  attr = PURE_ATTR;
	if(agent_attr == MINE_ATTR  && pannel_attr == ENEMY_ATTR)
		attr = ENEMY_ATTR;
	if(agent_attr == ENEMY_ATTR && pannel_attr == MINE_ATTR)
		attr = ENEMY_ATTR;
	return attr;
}

const uint_fast32_t Astar::heuristic(const std::pair<uint_fast32_t, uint_fast32_t>& coord, const std::pair<uint_fast32_t, uint_fast32_t>& goal) const{
	const uint_fast32_t dx = std::abs((int_fast32_t)(goal.first - coord.first));
	const uint_fast32_t dy = std::abs((int_fast32_t)(goal.second - coord.second));
	const double distance = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
	return distance;
}

void Astar::initNode(const Field& field){
	this->node.clear();
	this->node.resize(field.getWidth() * field.getHeight());
	for(size_t i = 0; i < field.getWidth(); i++)
		for(size_t j = 0; j < field.getHeight(); j++)
			this->node.at(j * field.getHeight() + i).coord = std::make_pair(i, j);
}

const bool Astar::comp(std::pair<Node*, Field>& lhs, std::pair<Node*, Field>& rhs){
	bool result = lhs.first->getScore() != rhs.first->getScore();
	return (result ? lhs.first->getScore() < rhs.first->getScore() : lhs.first->state_cost < rhs.first->state_cost);
}

const double Astar::searchRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal){
	std::vector<std::pair<Node*, Field>> open;
	Node *start , *current , *next;
	this->initNode(field);
	start =& this->node.at(field.agents.at(agent).getY() * field.getHeight() + field.agents.at(agent).getX());
	this->setStartNode(field, agent, goal, start);
	field.decided_route = this->decided_route;
	open.push_back(std::make_pair(start, field));
	while(!open.empty()){
		std::sort(open.begin(), open.end(), comp);
		current = open.at(0).first;
		Field current_field = open.at(0).second;
		
		
		if(this->branchingCondition(current))
			goto SKIP_NODE;

		
		if(current->coord == goal){
			if(this->endCondition(current))
				goto SKIP_NODE;
			return current_field.calcScore(MINE_ATTR) - current_field.calcScore(ENEMY_ATTR);
		}

		
		for(size_t i = 0; i < DIRECTION_SIZE - 2; i++){
			if(current_field.canMove(current_field.agents.at(agent), (Direction)i)){
				Field next_field = current_field;
				next_field.agents.at(agent).move((Direction)i);
				this->greedyMove(next_field, agent, current->move_num);
				this->decidedMove(next_field, agent, next_field.decided_route);
				//next_field.applyNextAgents();
				next =& this->node.at(next_field.agents.at(agent).getY() * field.getHeight() + next_field.agents.at(agent).getX());
				if(current->coord == next->coord){
					next_field.agents.at(agent).move((Direction)i);
					this->greedyMove(next_field, agent, current->move_num + 1);
					this->decidedMove(next_field, agent, next_field.decided_route);
					//next_field.applyNextAgents();
					next =& this->node.at(next_field.agents.at(agent).getY() * field.getHeight() + next_field.agents.at(agent).getX());
					if(next->status == Node::NONE && next->coord != current->coord){
						this->setNextNode(next_field, agent, goal, current, next);
						next->move_cost = current->move_cost + 2;
						open.push_back(std::make_pair(next, next_field));
					}
				}
				else{
					if(next->status == Node::NONE){
						this->setNextNode(next_field, agent, goal, current, next);
						next->move_cost = current->move_cost + 1;
						open.push_back(std::make_pair(next, next_field));
					}
				}
			}
		}
	SKIP_NODE:
		current->status = Node::CLOSED;
		open.erase(open.begin());
	}
	return - INT_MAX;
}


void Astar::setStartNode(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* start){
	start->status              = Node::OPEN;
	start->move_cost           = 0;
	start->state_cost          = field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	start->heuristic           = this->heuristic(start->coord, goal);
	start->is_on_decided_route = 0;
	start->move_num            = 0;
	if(field.agents.at(agent).getAttr() != field.at(start->coord.first, start->coord.second)->getAttr())
		start->value = field.at(start->coord.first, start->coord.second)->getValue();
	else
		start->value = -1;
}

void Astar::setNextNode(Field& next_field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* current, Node* next){
	next->status               = Node::OPEN;
	next->state_cost           = current->state_cost + next_field.calcScore(ENEMY_ATTR) - next_field.calcScore(MINE_ATTR);
	next->heuristic            = this->heuristic(next->coord, goal);
	next->is_on_decided_route  = current->is_on_decided_route + this->isOnDecidedRoute(next_field, agent, next->coord);
	next->move_num             = current->move_num + 1;
	next->parent               = current;
	
	if(next_field.agents.at(agent).getAttr() != next_field.at(next->coord.first, next->coord.second)->getAttr())
		next->value = next_field.at(next->coord.first, next->coord.second)->getValue();
	else
		next->value = -1;
}

const bool Astar::branchingCondition(Node* current) const{
	if(current->move_cost > 23)
		return true;
	if(current->value < -6)
		return true;
	
	return false;
}

const bool Astar::endCondition(Node* current) const{
	return (current->move_cost <= 3);
}

void Astar::searchBestRoute(Field& field, const uint_fast32_t agent){
	int_fast32_t value, max = - INT_MAX;
	std::pair<uint_fast32_t, uint_fast32_t> goal;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route;
	this->setSearchTarget(field, agent);
	std::for_each(this->search_target.begin(), this->search_target.end(), [&, this](auto& coord){
			value = this->searchRoute(field, agent, coord);
			if(value > max){
				max = value;
				goal = coord;
				route = this->makeRoute(field, agent, coord);
			}
		});
	this->decided_route.at(agent) = route;
	this->decided_goal.at(agent)  = goal;
	this->printRoute(route);
}

void 	Astar::search(Field& field, const uint_fast32_t attr){
	this->decided_route.clear();
	this->decided_route.resize(field.agents.size());
	this->decided_goal.clear();
	this->decided_goal.resize(field.agents.size());
	this->decided_coord.clear();
	
	for(size_t i = 0; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == attr)
			this->searchBestRoute(field, i);
}

const std::vector<std::pair<uint_fast32_t, uint_fast32_t>> Astar::makeRoute(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal){
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route;
	std::pair<uint_fast32_t, uint_fast32_t> from;
	route.push_back(goal);
	this->decided_coord.push_back(goal);
	from = this->node.at(goal.second * field.getHeight() + goal.first).parent->coord;
	route.push_back(from);
	while(true){
		if(this->node.at(from.second * field.getHeight() + from.first).parent == nullptr)
			break;
		from = this->node.at(from.second * field.getHeight() + from.first).parent->coord;
	  route.push_back(from);
		auto result = std::find(this->decided_coord.begin(), this->decided_coord.end(), from);
		if(result == this->decided_coord.end())
			this->decided_coord.push_back(from);
	}
	std::reverse(route.begin(), route.end());
	return route;
}

const void Astar::printGoal(Field& field, const uint_fast32_t attr) const{
	glPointSize(point_size - 10);
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_POINTS);
	for(size_t i = 0; i < this->decided_goal.size(); i++){
		if(field.agents.at(i).getAttr() == attr){
			glVertex2i(half + cell_size * this->decided_goal.at(i).first, half + cell_size * this->decided_goal.at(i).second);
		}
	}
	glEnd();
	glFlush();

	/*
	glPointSize(point_size);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	std::for_each(this->search_target.begin(), this->search_target.end(), [&, this](auto& coord){
			glVertex2i(half + cell_size * coord.first, half + cell_size * coord.second);
			std::cout<< "(" << coord.first << "," << coord.second << ")" << std::endl;
		});
	glEnd();
	glFlush();
	*/
}

const void Astar::printRoute(std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route){	
	glPointSize(point_size);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	std::for_each(route.begin(), route.end(), [&, this](auto& coord){
			glVertex2i(half + cell_size * coord.first, half + cell_size * coord.second);
		});
	glEnd();
	glFlush();
}

void Astar::singleMove(Field& field, const uint_fast32_t agent){
	Direction direction;




	
}

void Astar::mineMove(Field& field){
	this->search(field, MINE_ATTR);
	this->printGoal(field, MINE_ATTR);
}

void Astar::enemyMove(Field& field){
	this->search(field, ENEMY_ATTR);
	this->printGoal(field, ENEMY_ATTR);
}

void Astar::move(Field *field, const uint_fast32_t attr){
	Field obj = static_cast<Field> (*field);
	this->setAverageScore(obj);
	this->search(obj, MINE_ATTR);
	this->printGoal(obj, MINE_ATTR);
}

