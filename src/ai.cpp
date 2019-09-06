#include "ai.hpp"

//----------------AI--------------

AI::AI(){
}

AI::~AI(){
}

const bool AI::MineComp(std::pair<Field, Field>& lhs, std::pair<Field, Field>& rhs){
	int_fast32_t left_score  = lhs.second.calcScore(MINE_ATTR) - lhs.second.calcScore(ENEMY_ATTR);
	int_fast32_t right_score = rhs.second.calcScore(MINE_ATTR) - rhs.second.calcScore(ENEMY_ATTR);	
	
	bool result = left_score != right_score;
	return (result ? left_score > right_score : left_score > right_score);
}

const bool AI::EnemyComp(std::pair<Field, Field>& lhs, std::pair<Field, Field>& rhs){
	int_fast32_t left_score  = lhs.second.calcScore(ENEMY_ATTR) - lhs.second.calcScore(MINE_ATTR);
	int_fast32_t right_score = rhs.second.calcScore(ENEMY_ATTR) - rhs.second.calcScore(MINE_ATTR);
	
	bool result = left_score != right_score;
	return (result ? left_score > right_score : left_score > right_score);
}


//----------------Random--------------

Random::Random(){
	this->random = XorOshiro128p(time(NULL));
}

Random::~Random(){
}

void Random::init(const Field* field){
}

void Random::init(const Field& field){
}

void Random::move(Field *field, const uint_fast32_t attr){
	Field tmp = static_cast<Field> (*field);
	uint_fast32_t count = 0;
	Direction direction;
	
	for(auto &i: tmp.agents) {
		if(i.getAttr() == attr){
RE_CONSIDER:
			if(count++ > 10)
				break;
			direction = (Direction)(this->random(DIRECTION_SIZE - 3));
			if(tmp.canMove(i, direction))
				i.move(direction);
			else
				goto RE_CONSIDER;
		}
	}
	
	*field = tmp;	
}

//----------------Greedy--------------

Greedy::Greedy(){
	this->random = XorOshiro128p(time(NULL));
}

Greedy::~Greedy(){	
}

void Greedy::init(const Field* field){
}

void Greedy::init(const Field& field){
}

void Greedy::singleMove(Field& field, const uint_fast32_t agent){
	int_fast32_t max_score     = - INT_MAX;
	int_fast32_t next_score    = - INT_MAX;
	int_fast32_t current_score;
	
	if(field.agents.at(agent).getAttr() == MINE_ATTR)
		current_score = field.calcScore(MINE_ATTR) - field.calcScore(ENEMY_ATTR);
	else
		current_score = field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	
	Direction direction      = STOP;
	
	for(size_t i = 0; i < DIRECTION_SIZE - 2; i++){
		if(field.canMove(field.agents.at(agent), (Direction)i)){
			next_score = this->nextScore(field, agent, (Direction)i);
			if(next_score <= current_score)
				continue;
			if(next_score > max_score){
				max_score = next_score;
				direction = (Direction)i;
			}
		}
	}
	
	if(max_score <= current_score)
		this->randomMove(field, agent, field.agents.at(agent).getX(), field.agents.at(agent).getY());
	else
		field.agents.at(agent).move(direction);
}

void Greedy::randomMove(Field& field, const uint_fast32_t agent, const uint_fast32_t x, const uint_fast32_t y){
	Direction direction = STOP;
	uint_fast32_t count = 0;
	while(true){
		if(count++ > 10)
			break;
		direction = (Direction)(this->random(DIRECTION_SIZE - 3));
		if(field.at(x + this->vec_x.at(direction), y + this->vec_y.at(direction))->getValue() < -5)
			continue;
		if(field.canMove(field.agents.at(agent), direction)){
			field.agents.at(agent).move(direction);
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
	std::vector<std::thread> threads;
	Field tmp = static_cast<Field> (*field);

	for(size_t i =0; i < tmp.agents.size(); i++)
		if(tmp.agents.at(i).getAttr() == attr)
			this->singleMove(tmp, i);
	
	*field = tmp;
}

//----------------BeamSearch--------------

BeamSearch::BeamSearch(){
}

BeamSearch::~BeamSearch(){	
}

void BeamSearch::init(const Field* field){
	;
}

void BeamSearch::init(const Field& field){
	;
}

Field BeamSearch::search(Field* field, const uint_fast32_t agent, uint_fast32_t depth){
	if(depth == 0 || field->checkEnd())
		return *field;
	
	std::vector<std::pair<Field, Field>> fields;
	
	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++) {
		if(field->canMove(field->agents.at(agent), (Direction)i)){
			Field fbuf = *field;
			fbuf.agents.at(agent).move((Direction)i);
			
			/*
			if(field->agents.at(agent).getAttr() == MINE_ATTR)
				greedy.move(fbuf, ENEMY_ATTR);
			else
				greedy.move(fbuf, MINE_ATTR);
			*/
			
			fbuf.applyNextAgents();
			fields.emplace_back(fbuf, fbuf);
		}
	}
	
	for(size_t i = 0; i < fields.size(); i++)
		fields.at(i).second = this->search(&fields.at(i).first, agent, depth - 1);

	if(field->agents.at(agent).getAttr() == MINE_ATTR)
		std::sort(fields.begin(), fields.end(), MineComp);
	else
		std::sort(fields.begin(), fields.end(), EnemyComp);

	if(fields.size() > beam_width)
		fields.erase(fields.begin() + beam_width, fields.end());
	
	return fields.at(0).first;
}
	
void BeamSearch::singleMove(Field& field, const uint_fast32_t agent){
	Field current_field = field;
	Field next_field    = this->search(&current_field, agent, beam_depth);
	
	std::pair<uint_fast32_t, uint_fast32_t> current_coord = std::make_pair(current_field.agents.at(agent).getX(), current_field.agents.at(agent).getY()); 
	std::pair<uint_fast32_t, uint_fast32_t> next_coord    = std::make_pair(next_field.agents.at(agent).getnextbufX(), next_field.agents.at(agent).getnextbufY()); 

	Direction direction = this->changeDirection(current_coord, next_coord);
	
	if(field.canMove(field.agents.at(agent), direction))
		field.agents.at(agent).move(direction);
}

void BeamSearch::move(Field* field, const uint_fast32_t attr){
	Field tmp = static_cast<Field> (*field);

	for(size_t i =0; i < tmp.agents.size(); i++)
		if(tmp.agents.at(i).getAttr() == attr)
			this->singleMove(tmp, i);
	
	*field = tmp;
}

//----------------BreadthForceSearch--------------

BreadthForceSearch::BreadthForceSearch(){
}

BreadthForceSearch::~BreadthForceSearch(){
}

void BreadthForceSearch::init(const Field* field){
	;
}

void BreadthForceSearch::init(const Field& field){
	;
}

Field BreadthForceSearch::search(Field* field, const uint_fast32_t agent,  uint_fast32_t depth){
	if(depth == 0 || field->checkEnd())
		return *field;
	
	std::vector<std::pair<Field, Field>> fields;
	
	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++) {
		if(field->canMove(field->agents.at(agent), (Direction)i)){
			Field fbuf = *field;
			fbuf.agents.at(agent).move((Direction)i);
			
			/*
			if(field->agents.at(agent).getAttr() == MINE_ATTR)
				greedy.move(fbuf, ENEMY_ATTR);
			else
				greedy.move(fbuf, MINE_ATTR);
			*/
			
			fbuf.applyNextAgents();
			fields.emplace_back(fbuf, fbuf);
		}
	}
	
	for(size_t i = 0; i < fields.size(); i++)
		fields.at(i).second = this->search(&fields.at(i).first, agent, depth - 1);

	if(field->agents.at(agent).getAttr() == MINE_ATTR)
		std::sort(fields.begin(), fields.end(), MineComp);
	else
		std::sort(fields.begin(), fields.end(), EnemyComp);
	
	return fields.at(0).first;
}

void BreadthForceSearch::singleMove(Field& field, const uint_fast32_t agent){
	Field current_field = field;
	Field next_field    = this->search(&field, agent, bfs_depth);
	
	std::pair<uint_fast32_t, uint_fast32_t> current_coord = std::make_pair(current_field.agents.at(agent).getX(), current_field.agents.at(agent).getY()); 
	std::pair<uint_fast32_t, uint_fast32_t> next_coord    = std::make_pair(next_field.agents.at(agent).getX(), next_field.agents.at(agent).getY()); 

	Direction direction = this->changeDirection(current_coord, next_coord);
	if(field.canMove(field.agents.at(agent), direction))
		field.agents.at(agent).move(direction);
}

void BreadthForceSearch::move(Field *field, const uint_fast32_t attr){
	Field tmp = static_cast<Field> (*field);
	
	for(size_t i =0; i < tmp.agents.size(); i++)
		if(tmp.agents.at(i).getAttr() == attr)
			this->singleMove(tmp, i);
	
	*field = tmp;
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
	if(move_num > greedy_count)
		return;

	if(field.agents.at(agent).getAttr() == MINE_ATTR)
		this->greedy.move(&field, ENEMY_ATTR);
	else
		this->greedy.move(&field, MINE_ATTR);
	
	/*
	for(size_t i = agent + 1; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == field.agents.at(agent).getAttr())
			this->greedy.singleMove(field, i);
	*/
}


void Astar::decidedMove(Field& field, const uint_fast32_t agent, std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>>& route){
	Direction direction;
	for(size_t i = 0; i < field.agents.size(); i++){
		if(i == agent)
			continue;
		if(route.at(i).size() < 2)
			continue;
		direction = this->changeDirection(route.at(i).at(0), route.at(i).at(1));
		if(field.canMove(field.agents.at(i), direction))
			field.agents.at(i).move(direction);
	}
	
	field.applyNextAgents();
	
	for(size_t i = 0; i < field.agents.size(); i++){
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
				condidate.emplace_back(std::make_pair(value, std::make_pair(j, i)));
		}
	}
	
	if(condidate.empty())
		return;
	
	std::sort(condidate.rbegin(), condidate.rend());
	
	for(size_t i = 0; i < std::thread::hardware_concurrency() * 2; i++){
		if(i >= condidate.size())
			break;
		this->search_target.emplace_back(condidate.at(i).second);
	}
}

const double Astar::goalEvaluation(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal){
	if(this->expectTarget(field, agent, goal))
		return false;
	return field.at(goal.first, goal.second)->getValue() + this->occupancyRate(field, agent, goal) * occpancy_weight - (this->isOnDecidedRoute(field, agent, goal) * is_on_decided_weight);
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
	/*
	if(this->isOnDecidedRoute(field, agent, coord))
		return true;
	*/
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
	
	if(mine_distance <= min_mine_distance || mine_distance >= max_mine_distance)
		return true;
	
	for(size_t i = 0; i < field.agents.size(); i++){
		if(agent == i)
			continue;
		if(field.agents.at(agent).getAttr() != field.agents.at(i).getAttr())
			continue;
		another_distance = this->heuristic(std::make_pair(x, y), coord);
		if(another_distance < mine_distance && another_distance <= min_agent_distance)
			return true;
	}
	
	return false;
}

const bool Astar::anotherGoalDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t distance;
	for(size_t i = 0; i < agent; i++){
		if(field.agents.at(agent).getAttr() == field.agents.at(i).getAttr()){
			distance = this->heuristic(coord, this->decided_goal.at(i));
			if(distance <= min_goal_distance)
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

const double Astar::heuristic(const std::pair<uint_fast32_t, uint_fast32_t>& coord, const std::pair<uint_fast32_t, uint_fast32_t>& goal) const{
	const uint_fast32_t dx = std::abs((int_fast32_t)(goal.first - coord.first));
	const uint_fast32_t dy = std::abs((int_fast32_t)(goal.second - coord.second));
	const double distance  = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
	return distance;
}

void Astar::initNode(const Field& field, std::vector<Node>& node){
	node.clear();
	node.resize(field.getWidth() * field.getHeight());
	for(size_t i = 0; i < field.getWidth(); i++)
		for(size_t j = 0; j < field.getHeight(); j++)
			node.at(j * field.getWidth() + i).coord = std::make_pair(i, j);
			//node.at(j * field.getHeight() + i).coord = std::make_pair(i, j);
}

const bool Astar::comp(std::pair<Node*, Field>& lhs, std::pair<Node*, Field>& rhs){
	bool result = lhs.first->getScore() != rhs.first->getScore();
	return (result ? lhs.first->getScore() < rhs.first->getScore() : lhs.first->state_cost < rhs.first->state_cost);
}

const std::tuple<int_fast32_t, std::vector<Node>, std::pair<uint_fast32_t, uint_fast32_t>> Astar::searchRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal){
	std::vector<std::pair<Node*, Field>> open;
	Node *start, *current , *next;
	int_fast32_t score;
	std::vector<Node> node;
	
	this->initNode(field, node);
	
	start =& node.at(field.agents.at(agent).getY() * field.getWidth() + field.agents.at(agent).getX());
	this->setStartNode(field, agent, goal, start);
	field.decided_route = this->decided_route;
	open.emplace_back(std::make_pair(start, field));
	
	while(!open.empty()){
		std::sort(open.begin(), open.end(), comp);
		current = open.at(0).first;
		Field current_field = open.at(0).second;
		
		if(this->branchingCondition(current))
			goto SKIP_NODE;
		
		if(current->coord == goal){
			if(this->endCondition(current))
				goto SKIP_NODE;
			if(current_field.agents.at(agent).getAttr() == MINE_ATTR)
				score = current_field.calcScore(MINE_ATTR) - current_field.calcScore(ENEMY_ATTR);
			else
				score = current_field.calcScore(ENEMY_ATTR) - current_field.calcScore(MINE_ATTR);
			return std::make_tuple(score, node, goal);
		}
		
		for(size_t i = 0; i < DIRECTION_SIZE - 2; i++){
			if(current_field.canMove(current_field.agents.at(agent), (Direction)i)){
				Field next_field = current_field;
				next_field.agents.at(agent).move((Direction)i);
				//this->greedyMove(next_field, agent, current->move_num);
				this->decidedMove(next_field, agent,  next_field.decided_route);
				next_field.applyNextAgents();
				//next =& node.at(next_field.agents.at(agent).getY() * field.getHeight() + next_field.agents.at(agent).getX());
				next =& node.at(next_field.agents.at(agent).getY() * field.getWidth() + next_field.agents.at(agent).getX());
				
				if(current->coord == next->coord){
					next_field.agents.at(agent).move((Direction)i);
					//this->greedyMove(next_field, agent, current->move_num + 1);
				  this->decidedMove(next_field, agent,  next_field.decided_route);
					next_field.applyNextAgents();
					//next =& node.at(next_field.agents.at(agent).getY() * field.getHeight() + next_field.agents.at(agent).getX());
					next =& node.at(next_field.agents.at(agent).getY() * field.getWidth() + next_field.agents.at(agent).getX());
					
					if(current->coord == next->coord)
						continue;
					
					if(next->status == Node::NONE && next->coord != current->coord){
						this->setNextNode(next_field, agent, goal, current, next);
						next->move_cost = current->move_cost + 2;
						open.emplace_back(std::make_pair(next, next_field));
					}
				}
				else{
					if(next->status == Node::NONE){
						this->setNextNode(next_field, agent, goal, current, next);
						next->move_cost = current->move_cost + 1;
						open.emplace_back(std::make_pair(next, next_field));
					}
				}
			}
		}
		
	SKIP_NODE:
		current->status = Node::CLOSED;
		open.erase(open.begin());
	}

	return std::make_tuple(- INT_MAX, node, goal);
}

void Astar::setStartNode(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* start){
	start->status              = Node::OPEN;
	start->move_cost           = 0;
	
	if(field.agents.at(agent).getAttr() == MINE_ATTR)
		start->state_cost        = field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	else
		start->state_cost        = field.calcScore(MINE_ATTR) - field.calcScore(ENEMY_ATTR);
	
	start->heuristic           = this->heuristic(start->coord, goal);
	start->is_on_decided_route = 0;
	start->is_on_mine_panel    = 0;
	start->move_num            = 0;
	
	if(field.agents.at(agent).getAttr() != field.at(start->coord.first, start->coord.second)->getAttr())
		start->value = field.at(start->coord.first, start->coord.second)->getValue();
	else
		start->value = -1;
}

void Astar::setNextNode(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* current, Node* next){
	next->status               = Node::OPEN;
	
	if(field.agents.at(agent).getAttr() == MINE_ATTR)
		next->state_cost         = field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	else
		next->state_cost         = field.calcScore(MINE_ATTR) - field.calcScore(ENEMY_ATTR);
	
	next->heuristic            = this->heuristic(next->coord, goal);
	next->is_on_decided_route  = current->is_on_decided_route + this->isOnDecidedRoute(field, agent, next->coord);
	next->move_num             = current->move_num + 1;
	next->parent               = current;
	
	if(field.agents.at(agent).getAttr() == MINE_ATTR && field.at(next->coord.first, next->coord.second)->isMyPanel())
		next->is_on_mine_panel   = current->is_on_mine_panel + 1;
	if(field.agents.at(agent).getAttr() == ENEMY_ATTR && field.at(next->coord.first, next->coord.second)->isEnemyPanel())
		next->is_on_mine_panel   = current->is_on_mine_panel + 1;
	
	if(field.agents.at(agent).getAttr() != field.at(next->coord.first, next->coord.second)->getAttr())
		next->value = field.at(next->coord.first, next->coord.second)->getValue();
	else
		next->value = -1;
}

const bool Astar::branchingCondition(Node* current) const{
	if(current->move_cost >= max_move_cost)
		return true;
	if(current->value <= min_value)
		return true;
	
	return false;
}

const bool Astar::endCondition(Node* current) const{
	return (current->move_cost <= min_move_cost);
}


void Astar::multiThread(Field& field, const uint_fast32_t agent, std::pair<uint_fast32_t, uint_fast32_t> coord){
	std::tuple<int_fast32_t, std::vector<Node>, std::pair<uint_fast32_t, uint_fast32_t>> condidate;
	int_fast32_t score;
	std::vector<Node>* node;
	
	condidate = this->searchRoute(field, agent, coord);
	score     = std::get<0>(condidate);
	
	if(score > this->tentative_max_score){
		mtx.lock();
		
		this->tentative_max_score  =  score;
		node                       =& std::get<1>(condidate);		
		this->tentative_goal       =  std::get<2>(condidate);
		this->tentative_route      =  this->makeRoute(field, *node, agent, this->tentative_goal);

		mtx.unlock();
	}
}

void Astar::searchBestRoute(Field& field, const uint_fast32_t agent){
	std::vector<std::thread> threads;
	std::chrono::system_clock::time_point  start, end;
	double time;

	this->tentative_max_score = - INT_MAX;	
	this->setSearchTarget(field, agent);

	start = std::chrono::system_clock::now();
	
	std::for_each(this->search_target.begin(), this->search_target.end(), [&, this](auto& coord){
			threads.emplace_back(std::thread(&Astar::multiThread, this, std::ref(field), agent, std::ref(coord)));
		});
	
	for(auto& thread : threads)
		thread.join();

	end = std::chrono::system_clock::now();
	time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
	std::cout << time << "milliseconds" << std::endl;
	
	this->decided_route.at(agent) = this->tentative_route;
	this->decided_goal.at(agent)  = this->tentative_goal;
	this->printRoute(this->tentative_route);

	this->counter.at(agent) = 0;
}

void 	Astar::search(Field& field, const uint_fast32_t attr){
	this->init(field);
	for(size_t i = 0; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == attr)
			this->searchBestRoute(field, i);
}

const std::vector<std::pair<uint_fast32_t, uint_fast32_t>> Astar::makeRoute(Field& field, std::vector<Node>& node, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal){
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route;
	std::pair<uint_fast32_t, uint_fast32_t> from;
	
	route.push_back(goal);
	this->decided_coord.push_back(goal);
	//from = node.at(goal.second * field.getHeight() + goal.first).parent->coord;
	from = node.at(goal.second * field.getWidth() + goal.first).parent->coord;
	route.emplace_back(from);


	while(true){
		//if(node.at(from.second * field.getHeight() + from.first).parent == nullptr)
		if(node.at(from.second * field.getWidth() + from.first).parent == nullptr)
			break;
		//from = node.at(from.second * field.getHeight() + from.first).parent->coord;
		from = node.at(from.second * field.getWidth() + from.first).parent->coord;
	  route.emplace_back(from);
		auto result = std::find(this->decided_coord.begin(), this->decided_coord.end(), from);
		if(result == this->decided_coord.end())
			this->decided_coord.emplace_back(from);
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

void Astar::chooseAlgorithm(Field& field, const uint_fast32_t agent){
	if(field.getTurn() >= field.getMaxTurn() - beam_depth){
		this->breadth_force_search.singleMove(field, agent);
		//this->beam_search.singleMove(field, agent);
		
		this->decided_route.at(agent).clear();
		return;
	}
	
	this->singleMove(field, agent);
}

void Astar::singleMove(Field& field, const uint_fast32_t agent){
	Direction direction;
	const uint_fast32_t agentX = field.agents.at(agent).getX();
	const uint_fast32_t agentY = field.agents.at(agent).getY();

	this->decided_coord.clear();

	if(this->decided_route.at(agent).size() <= 1 || this->counter.at(agent) == search_count)
		this->searchBestRoute(field, agent);

	if(this->decided_route.at(agent).empty()){
		this->greedy.singleMove(field, agent);
		//this->beam_search.singleMove(field, agent);
		return;
	}
	
	if(this->decided_route.at(agent).at(0).first == agentX && this->decided_route.at(agent).at(0).second == agentY)
		this->decided_route.at(agent).erase(this->decided_route.at(agent).begin());
	
	auto result = std::find(this->next_coord.begin(), this->next_coord.end(), std::make_pair(this->decided_route.at(agent).at(0).first, this->decided_route.at(agent).at(0).second));
	if(result != this->next_coord.end())
		this->searchBestRoute(field, agent);
	
	if(this->decided_route.at(agent).empty()){
		this->greedy.singleMove(field, agent);
		//this->beam_search.singleMove(field, agent);
		return;
	}


	this->next_coord.push_back(std::make_pair(this->decided_route.at(agent).at(0).first, this->decided_route.at(agent).at(0).second));
	
	direction = this->changeDirection(std::make_pair(agentX, agentY), std::make_pair(this->decided_route.at(agent).at(0).first, this->decided_route.at(agent).at(0).second));
	
	if(field.canMove(field.agents.at(agent), direction)){
		field.agents.at(agent).move(direction);
		this->counter.at(agent)++;
	}
}

void Astar::init(const Field* field){
	this->decided_route.clear();
	this->decided_route.resize(field->agents.size());
	this->decided_goal.clear();
	this->decided_goal.resize(field->agents.size());
	this->decided_coord.clear();
	
	this->counter.clear();
	this->counter.resize(field->agents.size());
}

void Astar::init(const Field& field){
	this->decided_route.clear();
	this->decided_route.resize(field.agents.size());
	this->decided_goal.clear();
	this->decided_goal.resize(field.agents.size());
	this->decided_coord.clear();

	this->counter.clear();
	this->counter.resize(field.agents.size());
}

void Astar::move(Field *field, const uint_fast32_t attr){
	/*
	Field obj = static_cast<Field> (*field);
	this->init(obj);
	this->setAverageScore(obj);
	this->search(obj, attr);
	this->printGoal(obj, attr);
	*/

	Field tmp = static_cast<Field> (*field);
	this->decided_coord.clear();
	this->next_coord.clear();
	
	for(size_t i = 0; i < tmp.agents.size(); i++)
		if(tmp.agents.at(i).getAttr() == attr)
			this->chooseAlgorithm(tmp, i);
	*field = tmp;
}
