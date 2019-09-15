#include "ai.hpp"

//----------------AI--------------

AI::AI(){
}

AI::~AI(){
}

int_fast32_t AI::nextScore(Field field, const uint_fast32_t agent, const Direction direction) const{
	field.agents.at(agent).move(direction);
	field.applyNextAgents();
	if(field.agents.at(agent).getAttr() == MINE_ATTR)
		return field.calcScore(MINE_ATTR) - field.calcScore(ENEMY_ATTR);
	else
		return field.calcScore(ENEMY_ATTR) - field.calcScore(MINE_ATTR);
	return false;
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
			direction = (Direction)(this->random(DIRECTION_SIZE - 2));
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

void Greedy::move(Field *field, const uint_fast32_t attr){
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
	
	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
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

//----------------SimpleMove--------------

SimpleMove::SimpleMove(){
	
}

SimpleMove::~SimpleMove(){
	
}

void SimpleMove::greedyMove(Field& field, const uint_fast32_t agent, const uint_fast32_t move_num, const std::vector<std::pair<uint_fast32_t, uint_fast32_t>>& decided_coord){
	if(move_num > greedy_count)
		return;
	
	Direction direction = STOP;
	uint_fast32_t attr = field.agents.at(agent).getAttr();

	this->next_coord.clear();
	
	for(size_t i = 0; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() != field.agents.at(agent).getAttr())
			this->greedySingleMove(field, i, attr, decided_coord);

	for(size_t i = agent + 1; i < field.agents.size(); i++)
		if(field.agents.at(i).getAttr() == field.agents.at(agent).getAttr())
			this->greedySingleMove(field, i, attr, decided_coord);
}

void SimpleMove::greedySingleMove(Field& field, const uint_fast32_t agent, const uint_fast32_t attr, const std::vector<std::pair<uint_fast32_t, uint_fast32_t>>& decided_coord){
	Direction direction = STOP;
	uint_fast32_t x, y;
	int_fast32_t score = -INT_MAX, max_score = -INT_MAX;

	x = field.agents.at(agent).getX();
	y = field.agents.at(agent).getY();

	
	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		if(field.canMove(field.agents.at(agent), i)){
			score = field.at(x + this->vec_x.at(i), y + this->vec_y.at(i))->getValue();

			if(field.agents.at(i).getAttr() == attr){
				auto result = std::find(decided_coord.begin(), decided_coord.end(), std::make_pair(x + this->vec_x.at(i), y + this->vec_y.at(i)));
				if(result != decided_coord.end())
					score -= 50;
			}
			else{
				auto result = std::find(this->next_coord.begin(), this->next_coord.end(), std::make_pair(x + this->vec_x.at(i), y + this->vec_y.at(i)));
				if(result != this->next_coord.end())
					continue;
			}

			if(field.at(x + this->vec_x.at(i), y + this->vec_y.at(i))->getAttr() == field.agents.at(agent).getAttr())
				score -= 100;
			
			if(score > max_score){
				max_score = score;
				direction = (Direction)i;
			}
		}
	}

	field.agents.at(agent).move(direction);
	
	this->next_coord.emplace_back(std::make_pair(x + this->vec_x.at(direction), y + this->vec_y.at(direction)));
}

void SimpleMove::init(const Field* field){
	
}

void SimpleMove::init(const Field& field){
	
}
	
void SimpleMove::move(Field* field, const uint_fast32_t attr){
	
}

//----------------A*algorithm--------------

Astar::Astar(){
}

Astar::~Astar(){
}

void Astar::greedyMove(Field& field, const uint_fast32_t agent, const uint_fast32_t move_num){
	simple_move.greedyMove(field, agent, move_num, this->decided_coord);
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
	for(size_t i = 0; i < field.getHeight(); i++) {
		for(size_t j = 0; j < field.getWidth(); j++) {
			value = this->goalEvaluation(field, agent, std::make_pair(j, i));
			if(value == false)
				continue;
			condidate.emplace_back(std::make_pair(value, std::make_pair(j, i)));
		}
	}
	std::sort(condidate.begin(), condidate.end(), _comp);
	for(size_t i = 0; i < astar_depth/*std::thread::hardware_concurrency() * 2*/; i++){
		if(i >= condidate.size())
			break;
		this->search_target.emplace_back(condidate.at(i).second);
	}
}

const bool Astar::_comp(std::pair<double, std::pair<uint_fast32_t, uint_fast32_t>>& lhs ,std::pair<double, std::pair<uint_fast32_t, uint_fast32_t>>& rhs){
	bool result = lhs.first != rhs.first;
	return (result ? lhs.first > rhs.first : lhs.first > rhs.first);
}

const double Astar::goalEvaluation(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal){
	if(this->expectTarget(field, agent, goal))
		return false;

	
	bool is_my_pannel     = false;
	bool is_angle         = false;
	bool is_side          = false;
	bool is_inside_closed = false;

	//関数化する
	if(this->whosePanel(field, agent, goal) == MINE_ATTR)
		is_my_pannel     = true;
	if(this->isSideOrAngle(field, goal) == ANGLE_COORD)
		is_angle         = true;
	if(this->isSideOrAngle(field, goal) == SIDE_COORD)
		is_side          = true;

	//fieldを動かしてから考えたほうが良い
	if(field.is_inside_closed(goal))
		is_inside_closed = true;

	
	return field.at(goal.first, goal.second)->getValue() + this->occupancyRate(field, agent, goal) * occpancy_weight - (this->isOnDecidedRoute(field, agent, goal) * is_on_decided_weight) - (is_my_pannel * is_my_pannel_weight) - (is_angle * is_angle_weight) - (is_side * is_side_weight)- (is_inside_closed * is_inside_closed_weight);
}

const uint_fast32_t Astar::occupancyRate(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t count = 0;
	for(size_t i = 0; i < DIRECTION_SIZE - 1; i++){
		if(this->whosePanel(field, agent, std::make_pair(coord.first + this->vec_x.at(i), coord.second + this->vec_y.at(i))) == ENEMY_ATTR)
			count++;
	}
	return count;
}

const uint_fast32_t Astar::whosePanel(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t	agent_attr  = field.at(coord.first, coord.second)->getAttr();
	uint_fast32_t	pannel_attr = field.at(coord.first, coord.second)->getAttr();
	uint_fast32_t attr = MINE_ATTR;
	
	if(pannel_attr == PURE_ATTR)
	  attr = PURE_ATTR;
	if(agent_attr == MINE_ATTR  && pannel_attr == ENEMY_ATTR)
		attr = ENEMY_ATTR;
	if(agent_attr == ENEMY_ATTR && pannel_attr == MINE_ATTR)
		attr = ENEMY_ATTR;
	return attr;
}

const uint_fast32_t Astar::isSideOrAngle(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	if(coord.first == 0 && coord.second == 0 || coord.first == 0 && coord.second == field.getHeight() - 1 || coord.first == field.getWidth() - 1 && coord.second == 0 || coord.first == field.getWidth() - 1 && coord.second == field.getHeight() - 1)
		return ANGLE_COORD;
	if(coord.first == 0 || coord.second == 0 || coord.first == field.getWidth() - 1 || coord.second == field.getHeight() - 1)
		return SIDE_COORD;
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

const bool Astar::isMyPannel(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	
}

const bool Astar::isAngleCoord(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	
}

const bool Astar::isSideCoord(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	
}

const bool Astar::isInsideClosed(Field field, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	
}

const bool Astar::expectTarget(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	if(field.agents.at(agent).getX() == coord.first && field.agents.at(agent).getY() == coord.second)
		return true;
	if(field.at(coord.first, coord.second)->getValue() < this->average_score)
		return true;
	if(this->anotherAgentDistance(field, agent, coord))
		return true;
	if(this->anotherGoalDistance(field, agent, coord))
		return true;
	
	return false;
}

const bool Astar::anotherAgentDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	uint_fast32_t x = field.agents.at(agent).getX();
	uint_fast32_t y = field.agents.at(agent).getY();
	double another_distance, mine_distance = this->heuristic(std::make_pair(x, y), coord);
	
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

const double Astar::heuristic(const std::pair<uint_fast32_t, uint_fast32_t>& coord, const std::pair<uint_fast32_t, uint_fast32_t>& goal) const{
	const uint_fast32_t dx = std::abs((int_fast32_t)(goal.first - coord.first));
	const uint_fast32_t dy = std::abs((int_fast32_t)(goal.second - coord.second));
	const double distance  = std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
	return distance;
}

const bool Astar::isAdjacentAgent(const Field& field, const uint_fast32_t agent, const uint_fast32_t attr){
	if(attr == MINE_ATTR)
		return this->isAdjacentMineAgent(field, agent);
	if(attr == ENEMY_ATTR)
		return this->isAdjacentEnemyAgent(field, agent);
	return false;
}

const bool Astar::isAdjacentMineAgent(const Field& field, const uint_fast32_t agent){
	const uint_fast32_t x    = field.agents.at(agent).getX();
	const uint_fast32_t y    = field.agents.at(agent).getY();
	const uint_fast32_t attr = field.agents.at(agent).getAttr();
	bool flag = false;

	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& agent){
				if(attr == agent.getAttr()){
					if(x + this->vec_x.at(i) == agent.getX() && y + this->vec_y.at(i) == agent.getY()){
						flag = true;
						return;
					}
				}
			});
	}

	return flag;
}

const bool Astar::isAdjacentEnemyAgent(const Field& field, const uint_fast32_t agent){
	const uint_fast32_t x    = field.agents.at(agent).getX();
	const uint_fast32_t y    = field.agents.at(agent).getY();
	const uint_fast32_t attr = field.agents.at(agent).getAttr();
	bool flag = false;

	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& agent){
				if(attr != agent.getAttr()){
					if(x + this->vec_x.at(i) == agent.getX() && y + this->vec_y.at(i) == agent.getY()){
						flag = true;
						return;
					}
				}
			});
	}

	return flag;
}

const uint_fast32_t Astar::countAdjacentAgent(const Field& field, const uint_fast32_t agent, const uint_fast32_t attr){
	if(attr == MINE_ATTR)
		return this->countAdjacentMineAgent(field, agent);
	if(attr == ENEMY_ATTR)
		return this->countAdjacentEnemyAgent(field, agent);
	return 0;
}

const uint_fast32_t Astar::countAdjacentMineAgent(const Field& field, const uint_fast32_t agent){
	const uint_fast32_t x    = field.agents.at(agent).getX();
	const uint_fast32_t y    = field.agents.at(agent).getY();
	const uint_fast32_t attr = field.agents.at(agent).getAttr();
	uint_fast32_t count = 0;

	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& agent){
				if(attr == agent.getAttr())
					if(x + this->vec_x.at(i) == agent.getX() && y + this->vec_y.at(i) == agent.getY())
						count++;
			});
	}

	return count;
}
	
const uint_fast32_t Astar::countAdjacentEnemyAgent(const Field& field, const uint_fast32_t agent){
	const uint_fast32_t x    = field.agents.at(agent).getX();
	const uint_fast32_t y    = field.agents.at(agent).getY();
	const uint_fast32_t attr = field.agents.at(agent).getAttr();
	uint_fast32_t count = 0;

	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& agent){
				if(attr != agent.getAttr())
					if(x + this->vec_x.at(i) == agent.getX() && y + this->vec_y.at(i) == agent.getY())
						count++;
			});
	}

	return count;
}

const double Astar::averageDistanceAgent(const Field& field, const uint_fast32_t agent, const uint_fast32_t attr){
	if(attr == MINE_ATTR)
		return this->averageDistanceMineAgent(field, agent);
	if(attr == ENEMY_ATTR)
		return this->averageDistanceEnemyAgent(field, agent);
	return 0;
}

const double Astar::averageDistanceMineAgent(const Field& field, const uint_fast32_t agent){
	const uint_fast32_t x    = field.agents.at(agent).getX();
	const uint_fast32_t y    = field.agents.at(agent).getY();
	const uint_fast32_t attr = field.agents.at(agent).getAttr();
	uint_fast32_t sum = 0;

	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& agent){
				if(attr == agent.getAttr())
					sum += this->heuristic(std::make_pair(x, y), std::make_pair(agent.getX(), agent.getY()));
			});
	}
	return (double)sum / field.agents.size();
}

const double Astar::averageDistanceEnemyAgent(const Field& field, const uint_fast32_t agent){
	const uint_fast32_t x    = field.agents.at(agent).getX();
	const uint_fast32_t y    = field.agents.at(agent).getY();
	const uint_fast32_t attr = field.agents.at(agent).getAttr();
	uint_fast32_t sum = 0;

	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& agent){
				if(attr != agent.getAttr())
					sum += this->heuristic(std::make_pair(x, y), std::make_pair(agent.getX(), agent.getY()));
			});
	}
	return (double)sum / field.agents.size();
}

const uint_fast32_t Astar::countWithinRangeAgent(const Field& field, const uint_fast32_t agent, const double range, const uint_fast32_t attr){
	if(attr == MINE_ATTR)
		return this->countWithinRangeMineAgent(field, agent, range);
	if(attr == ENEMY_ATTR)
		return this->countWithinRangeEnemyAgent(field, agent, range);
	return 0;
}

const uint_fast32_t Astar::countWithinRangeMineAgent(const Field& field, const uint_fast32_t agent, const double range){
	const uint_fast32_t x    = field.agents.at(agent).getX();
	const uint_fast32_t y    = field.agents.at(agent).getY();
	const uint_fast32_t attr = field.agents.at(agent).getAttr();
	uint_fast32_t count = 0;
	double distance     = 0;

	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& agent){
				if(attr == agent.getAttr()){
					distance = this->heuristic(std::make_pair(x, y), std::make_pair(agent.getX(), agent.getY()));
					if(distance <= range) count++;
				}
			});
	}
	return count;
}

const uint_fast32_t Astar::countWithinRangeEnemyAgent(const Field& field, const uint_fast32_t agent, const double range){
	const uint_fast32_t x    = field.agents.at(agent).getX();
	const uint_fast32_t y    = field.agents.at(agent).getY();
	const uint_fast32_t attr = field.agents.at(agent).getAttr();
	uint_fast32_t count = 0;
	double distance     = 0;

	for(size_t i = 0; i < DIRECTION_SIZE - 3; i++){
		std::for_each(field.agents.begin(), field.agents.end(), [&, this](auto& agent){
				if(attr != agent.getAttr()){
					distance = this->heuristic(std::make_pair(x, y), std::make_pair(agent.getX(), agent.getY()));
					if(distance <= range) count++;
				}
			});
	}
	return count;
}

void Astar::initNode(const Field& field, std::vector<Node>& node){
	node.clear();
	node.resize(field.getWidth() * field.getHeight());
	for(size_t i = 0; i < field.getWidth(); i++)
		for(size_t j = 0; j < field.getHeight(); j++)
			node.at(j * field.getWidth() + i).coord = std::make_pair(i, j);
}

const bool Astar::comp(std::pair<Node*, Field>& lhs, std::pair<Node*, Field>& rhs){
	bool result = lhs.first->getScore() != rhs.first->getScore();
	return (result ? lhs.first->getScore() < rhs.first->getScore() : lhs.first->state_cost < rhs.first->state_cost);
}

std::pair<int_fast32_t, std::vector<Node>> Astar::searchRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, const uint_fast32_t max_move_cost){
	std::vector<std::pair<Node*, Field>> open;
	std::vector<Node> node;
	
	Node *start, *current , *next;
	Field current_field, next_field;
	int_fast32_t score;
	
	this->initNode(field, node);
	start =& node.at(field.agents.at(agent).getY() * field.getWidth() + field.agents.at(agent).getX());
	this->setStartNode(field, agent, goal, start);
	field.decided_route = this->decided_route;
	open.emplace_back(std::make_pair(start, field));
	
	while(!open.empty()){
		std::sort(open.begin(), open.end(), comp);
		current = open.at(0).first;
		current_field = open.at(0).second;
		
		if(this->branchingCondition(current, max_move_cost))
			goto SKIP_NODE;
		
		if(this->isTimeOver()){
			this->is_time_over = true;
			return std::make_pair(-INT_MAX, node);
		}
		
		if(current->coord == goal){
			if(this->endCondition(current))
				goto SKIP_NODE;
			if(current_field.agents.at(agent).getAttr() == MINE_ATTR)
				score = current_field.calcScore(MINE_ATTR) - current_field.calcScore(ENEMY_ATTR);
			else
				score = current_field.calcScore(ENEMY_ATTR) - current_field.calcScore(MINE_ATTR);
			return std::make_pair(score, node);
		}
		
		for(size_t i = 0; i < DIRECTION_SIZE - 2; i++){
			if(current_field.canMove(current_field.agents.at(agent), (Direction)i)){
				next_field = current_field;
				next_field.agents.at(agent).move((Direction)i);
				this->greedyMove(next_field, agent, current->move_num);
				this->decidedMove(next_field, agent,  next_field.decided_route);
				next =& node.at(next_field.agents.at(agent).getY() * field.getWidth() + next_field.agents.at(agent).getX());
				
				if(current->coord == next->coord){
					next_field.agents.at(agent).move((Direction)i);
					this->greedyMove(next_field, agent, current->move_num + 1);
				  this->decidedMove(next_field, agent,  next_field.decided_route);
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

	return std::make_pair(-INT_MAX, node);
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
	start->is_adjacent_agent   = 0;
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
	next->is_adjacent_agent    = current->is_adjacent_agent + this->isAdjacentAgent(field, agent, MINE_ATTR);
	next->move_num             = current->move_num + 1;
	next->parent               = current;
	
	if(field.agents.at(agent).getAttr() == MINE_ATTR && field.at(next->coord.first, next->coord.second)->isMyPanel())
		next->is_on_mine_panel   = current->is_on_mine_panel + 1;
	if(field.agents.at(agent).getAttr() == ENEMY_ATTR && field.at(next->coord.first, next->coord.second)->isEnemyPanel())
		next->is_on_mine_panel   = current->is_on_mine_panel + 1;

	if(field.agents.at(agent).getAttr() != field.at(next->coord.first, next->coord.second)->getAttr())
		next->value = field.at(next->coord.first, next->coord.second)->getValue() + current->value;
	else
		next->value = current->value;
}

const bool Astar::branchingCondition(Node* current , const uint_fast32_t max_move_cost) const{
	if(current->move_cost >= max_move_cost)
		return true;
	if(current->value <= -min_value)
		return true;
	
	return false;
}

const bool Astar::endCondition(Node* current) const{
	return (current->move_cost <= min_move_cost);
}

const bool Astar::isTimeOver() const{
	std::chrono::system_clock::time_point current;
	double_t time;
	
	current = std::chrono::system_clock::now();
	time    = std::chrono::duration_cast<std::chrono::milliseconds>(current - this->clock).count();
	if(time > search_time - grace_time)
		return true;
	
	return false;
}

void Astar::multiThread(Field field, const uint_fast32_t agent, std::pair<uint_fast32_t, uint_fast32_t> coord){
	std::pair<int_fast32_t, std::vector<Node>> condidate;
	int_fast32_t score;
	condidate = this->searchRoute(field, agent, coord, max_move);
	score     = condidate.first;
	if(score > this->tentative_max_score){
		this->tentative_max_score = score;
		this->tentative_goal      = coord;
		this->tentative_route     = this->makeRoute(field, condidate.second, agent, coord);
	}
}

void Astar::searchBestRoute(Field& field,const uint_fast32_t agent){
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route;
	std::chrono::system_clock::time_point  start, end;
	double time;
	
	start = std::chrono::system_clock::now();

	this->tentative_route.clear();
	this->tentative_max_score = - INT_MAX;	
	this->setSearchTarget(field, agent);
	
	std::for_each(this->search_target.begin(), this->search_target.end(), [&, this](auto& coord){
			this->multiThread(field, agent, coord);
			if(this->is_time_over)
				return;
		});
	
	if(this->is_time_over)
		return;
	
	end  = std::chrono::system_clock::now();
	time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << time << "milliseconds" << std::endl;

	if(this->tentative_max_score == -INT_MAX)
		return;

	this->decided_route.at(agent) = this->tentative_route;
	this->decided_goal.at(agent)  = this->tentative_goal;
	this->setDecidedCoord(this->tentative_route);
	this->current_score.at(agent) = this->tentative_max_score;

	
	this->printRoute(this->tentative_route);
	this->counter.at(agent) = 0;

	/*
	std::vector<std::thread> threads;
	std::chrono::system_clock::time_point  start, end;
	double time;
	
	this->setSearchTarget(field, agent);

	tentative_max_score = - INT_MAX;
	start = std::chrono::system_clock::now();
	
	std::for_each(this->search_target.begin(), this->search_target.end(), [&, this](auto& coord){
			threads.emplace_back(std::thread(&Astar::multiThread, this, std::ref(field), agent, std::ref(coord)));
		});
	
	for(auto& thread : threads)
		thread.join();
	
	end = std::chrono::system_clock::now();
	time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
	std::cout << time << "milliseconds" << std::endl;

	if(this->tentative_max_score == -INT_MAX){
		this->decided_route.at(agent).clear();
		return;
	}

	this->decided_route.at(agent) = this->tentative_route;
	this->decided_goal.at(agent)  = this->tentative_goal;

	this->setDecidedCoord(this->tentative_route);
	
	this->printRoute(this->tentative_route);
	this->counter.at(agent) = 0;
	*/
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
	from = node.at(goal.second * field.getWidth() + goal.first).parent->coord;
	route.emplace_back(from);

	while(true){
		if(node.at(from.second * field.getWidth() + from.first).parent == nullptr)
			break;
		from = node.at(from.second * field.getWidth() + from.first).parent->coord;
	  route.emplace_back(from);
	}
	
	std::reverse(route.begin(), route.end());
	return route;
}

void Astar::setDecidedCoord(const std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route){
	std::for_each(route.begin(), route.end(), [&, this](auto& coord){
			auto result = std::find(this->decided_coord.begin(), this->decided_coord.end(), coord);
			if(result == this->decided_coord.end())
				this->decided_coord.emplace_back(coord);
		});
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
	//全探索
	if(field.getTurn() >= field.getMaxTurn() - beam_depth){
		this->breadth_force_search.singleMove(field, agent);
		this->decided_route.at(agent) = std::vector<std::pair<uint_fast32_t, uint_fast32_t>>();
		return;
	}

	//時間処理
	if(this->is_time_over){
		this->beam_search.singleMove(field, agent);
		//this->greedy.singleMove(field, agent);
		this->decided_route.at(agent) = std::vector<std::pair<uint_fast32_t, uint_fast32_t>>();
		return;
	}

	/*
	//敵Agentが隣接してたら、BeamSearchを使う
	if(this->isAdjacentAgent(field, agent, ENEMY_ATTR)){
		this->beam_search.singleMove(field, agent);
    this->decided_route.at(agent) = std::vector<std::pair<uint_fast32_t, uint_fast32_t>>();
		return;
	}
	*/
	
	this->singleMove(field, agent);
}

void Astar::singleMove(Field& field, const uint_fast32_t agent){
	Direction direction;
	const uint_fast32_t agentX = field.agents.at(agent).getX();
	const uint_fast32_t agentY = field.agents.at(agent).getY();

	
	if(this->decided_route.at(agent).size() <= 1 || this->counter.at(agent) == search_count)
		this->searchBestRoute(field, agent);
	else{
		this->correctionRoute(field, agent);
		goto _SKIP;
	}

	//時間処理
	if(this->is_time_over)
		goto _TIME_OVER;

	if(this->decided_route.at(agent).empty()){
		this->beam_search.singleMove(field, agent);
		this->decided_route.at(agent) = std::vector<std::pair<uint_fast32_t, uint_fast32_t>>();
		return;
	}

	//時間処理
	if(this->is_time_over)
		goto _TIME_OVER;
	
 _SKIP:
	
	if(this->decided_route.at(agent).at(0).first == agentX && this->decided_route.at(agent).at(0).second == agentY)
		this->decided_route.at(agent).erase(this->decided_route.at(agent).begin());

	auto result = std::find(this->next_coord.begin(), this->next_coord.end(), std::make_pair(this->decided_route.at(agent).at(0).first, this->decided_route.at(agent).at(0).second));
	if(result != this->next_coord.end()){
		//this->searchBestRoute(field, agent);
		this->beam_search.singleMove(field, agent);
		this->decided_route.at(agent) = std::vector<std::pair<uint_fast32_t, uint_fast32_t>>();
	}

	//時間処理
	if(this->is_time_over)
		goto _TIME_OVER;

	//念のため
	if(this->decided_route.at(agent).empty()){
		this->beam_search.singleMove(field, agent);
		this->decided_route.at(agent) = std::vector<std::pair<uint_fast32_t, uint_fast32_t>>();
		return;
	}

	//時間処理
	if(this->is_time_over)
		goto _TIME_OVER;
	
	if(this->decided_route.at(agent).empty()){
		this->beam_search.singleMove(field, agent);
		this->decided_route.at(agent) = std::vector<std::pair<uint_fast32_t, uint_fast32_t>>();
		return;
	}
	
	this->next_coord.push_back(std::make_pair(this->decided_route.at(agent).at(0).first, this->decided_route.at(agent).at(0).second));
	
	direction = this->changeDirection(std::make_pair(agentX, agentY), std::make_pair(this->decided_route.at(agent).at(0).first, this->decided_route.at(agent).at(0).second));
	if(field.canMove(field.agents.at(agent), direction)){
		field.agents.at(agent).move(direction);
		this->counter.at(agent)++;
		return;
	}

 _TIME_OVER:
	this->beam_search.singleMove(field, agent);
	//this->greedy.singleMove(field, agent);
	this->decided_route.at(agent) = std::vector<std::pair<uint_fast32_t, uint_fast32_t>>();
}

void Astar::correctionRoute(Field& field, const uint_fast32_t agent){
	std::pair<int_fast32_t, std::vector<Node>> condidate;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route;
	int_fast32_t score;
	
	condidate = this->searchRoute(field, agent, this->decided_goal.at(agent), this->decided_route.at(agent).size());
	score     = condidate.first;
	
	if(score > this->current_score.at(agent)){
		route = this->makeRoute(field, condidate.second, agent, this->decided_goal.at(agent));		
		this->decided_route.at(agent) = route;
		this->setDecidedCoord(route);
		this->current_score.at(agent) = score;
		this->setDecidedCoord(this->tentative_route);

		//this->counter.at(agent)       = 0;
		this->printRoute(route);
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
	
	this->current_score.clear();
	this->current_score.resize(field->agents.size());
}

void Astar::init(const Field& field){
	this->decided_route.clear();
	this->decided_route.resize(field.agents.size());
	this->decided_goal.clear();
	this->decided_goal.resize(field.agents.size());
	this->decided_coord.clear();

	this->counter.clear();
	this->counter.resize(field.agents.size());
	
	this->current_score.clear();
	this->current_score.resize(field.agents.size());
}

void Astar::move(Field *field, const uint_fast32_t attr){
	
	/*
	Field obj = static_cast<Field> (*field);
	this->init(obj);
	this->setAverageScore(obj);
	this->search(obj, attr);
	this->printGoal(obj, attr);
	*/

	this->clock = std::chrono::system_clock::now();
	this->is_time_over = false;

	
	this->decided_coord.clear();
	this->next_coord.clear();

		
	Field tmp = static_cast<Field> (*field);

	
	this->setAverageScore(tmp);
	for(size_t i = 0; i < tmp.agents.size(); i++)
		if(tmp.agents.at(i).getAttr() == attr)
			this->chooseAlgorithm(tmp, i);
	*field = tmp;
}
