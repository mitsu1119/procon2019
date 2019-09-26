#pragma once

#include "agent.hpp"
#include "field.hpp"
#include "useful.hpp"

class Agent;
class Field;

class AI{
private:
public:

	const std::vector<int> vec_x = { 0, 1, 1, 1, 0,-1,-1, -1, 0};
	const std::vector<int> vec_y = {-1,-1, 0, 1, 1, 1, 0, -1, 0};

public:

	AI();
	~AI();

	virtual	void init(const Field* field) = 0;
	virtual	void init(const Field& field) = 0;
	virtual void move(Field *field, const uint_fast32_t attr) = 0;

	const Direction changeDirection(const std::pair<uint_fast32_t, uint_fast32_t>& now, const std::pair<uint_fast32_t, uint_fast32_t>& next) const;
	int_fast32_t nextScore(Field field, const uint_fast32_t agent, const Direction direction) const;
	static const bool MineComp(std::pair<Field, Field>& lhs, std::pair<Field, Field>& rhs);
	static const bool EnemyComp(std::pair<Field, Field>& lhs, std::pair<Field, Field>& rhs);

	void decidedMove(Field& field, const uint_fast32_t agent, std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>>& route);
	
};

inline const Direction AI::changeDirection(const std::pair<uint_fast32_t, uint_fast32_t>& now, const std::pair<uint_fast32_t, uint_fast32_t>& next) const{
	int dx = next.first - now.first;
	int dy = next.second - now.second;
	return xyToDirection(next.first - now.first, next.second - now.second);
}

class Random : public AI{
private:

	XorOshiro128p random;

public:

	Random();
	~Random();

	void init(const Field* field) override;
	void init(const Field& field) override;
	void move(Field *field, const uint_fast32_t attr) override;

};

class Greedy : public AI{
private:

	XorOshiro128p random;

public:

	Greedy();
	~Greedy();

	void init(const Field* field) override;
	void init(const Field& field) override;

	void singleMove(Field& field, const uint_fast32_t agent);
	void randomMove(Field& field, const uint_fast32_t agent, const uint_fast32_t x, const uint_fast32_t y);

	void move(Field *field, const uint_fast32_t attr) override;

};

constexpr uint_fast32_t beam_depth = 1;
constexpr uint_fast32_t beam_width = 3;

class BeamSearch : public AI{
private:

	Greedy greedy;
	Field search(Field *field, const uint_fast32_t agent, uint_fast32_t depth);
	
public:

	BeamSearch();
	~BeamSearch();

	void init(const Field* field) override;
	void init(const Field& field) override;

	void singleMove(Field& field, const uint_fast32_t agent);
	void move(Field* field, const uint_fast32_t attr) override;

};

constexpr uint_fast32_t bfs_depth = 3;

class BreadthForceSearch : public AI{
private:

	Greedy greedy;
	Field search(Field* field, const uint_fast32_t agent, uint_fast32_t depth);

public:

	BreadthForceSearch();
	~BreadthForceSearch();

	void init(const Field* field) override;
	void init(const Field& field) override;

	void singleMove(Field& field, const uint_fast32_t agent);
	void move(Field* field, const uint_fast32_t attr) override;

};

constexpr double_t move_weight                = 2;
constexpr double_t state_weight               = 55;
constexpr double_t heuristic_weight           = 5;
constexpr double_t value_weight               = 35;
constexpr double_t is_on_decided_route_weight = 80;
constexpr double_t is_on_mine_panel_weight    = 95;
constexpr double_t is_on_enemy_panel_weight   = 90;
constexpr double_t adjacent_agent_weight      = 10;
constexpr double_t average_distance_weght     = 40;

/*
static double_t move_weight = 1;
static double_t state_weight = 1;
static double_t heuristic_weight = 1;
static double_t value_weight = 1;
static double_t is_on_decided_route_weight = 1;
static double_t is_on_mine_panel_weight = 1;
static double_t is_on_enemy_panel_weight = 1;
static double_t adjacent_agent_weight = 1;
static double_t average_distance_weght = 1;
*/

class Node{
public:

	enum Status{
		NONE,
		OPEN,
		CLOSED
	};

	//------------ 評価値 ------------
	
	//ステータス
	Status status;
	//移動コスト
	int_fast32_t move_cost;
	//盤の状態によるコスト
	int_fast32_t state_cost;
	//ヒューリスティックコスト（推定コスト）
	int_fast32_t heuristic;
	//その地点での点数
	int_fast32_t  value;
	//確定ルートにかぶっているか？
  int_fast32_t is_on_decided_route;
	//パネル属性
	int_fast32_t is_on_mine_panel;
	int_fast32_t is_on_enemy_panel;
	//Agent同士が隣接しているかどうか
	int_fast32_t adjacent_agent;
	//Agent同士の平均距離
	double average_distance;

	
	//------------ 探索で使用 ------------
	
	int_fast32_t move_num;
	Node* parent;
	std::pair<uint_fast32_t, uint_fast32_t> coord;

public:

	Node();
	~Node();
	const double getScore() const;

};

inline const double Node::getScore() const{
	return (this->move_cost * move_weight) + (this->state_cost * state_weight) + (this->heuristic * heuristic_weight) + (this->is_on_decided_route * is_on_decided_route_weight) - (this->value * value_weight) + (this->is_on_mine_panel * is_on_mine_panel_weight) + (this->adjacent_agent * adjacent_agent_weight) - (this->average_distance * average_distance_weght) - (this->is_on_enemy_panel * is_on_enemy_panel_weight);
}

constexpr uint_fast32_t simple_beam_depth = 3;
constexpr uint_fast32_t simple_beam_width = 3;
constexpr uint_fast32_t simple_bfs_depth  = 3;

class SimpleMove : public AI{
private:

	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> next_coord;

private:

	Field beamSearch(Field* field, const uint_fast32_t agent, uint_fast32_t depth) const;
	Field breadthForceSearch(Field* field, const uint_fast32_t agent, uint_fast32_t depth) const;

public:

	SimpleMove();
	~SimpleMove();
	
	void greedyMove(Field& field, const uint_fast32_t agent, const std::vector<std::pair<uint_fast32_t, uint_fast32_t>>& decided_coord);
	void greedySingleMove(Field& field, const uint_fast32_t agent, const uint_fast32_t attr, const std::vector<std::pair<uint_fast32_t, uint_fast32_t>>& decided_coord);
	void greedyMove(Field& field, const uint_fast32_t agent);
	void greedySingleMove(Field& field, const uint_fast32_t agent, const uint_fast32_t attr);
	const Direction greedySingleMove(Field& field, const uint_fast32_t agent, const std::vector<std::pair<uint_fast32_t, uint_fast32_t>>& expect_coord) const;
	
	const Direction beamSearchSingleMove(Field field, const uint_fast32_t agent) const;
	void beamSearchMove(Field& field, const uint_fast32_t attr) const;

	const Direction breadthForceSearchSingleMove(Field& field, const uint_fast32_t agent) const;
	void breadthForceSearchSearchMove(Field& field, const uint_fast32_t attr) const;
	
	void init(const Field* field) override;
	void init(const Field& field) override;
	void move(Field* field, const uint_fast32_t attr) override;
	
};

constexpr uint_fast32_t greedy_count       = 8;
constexpr uint_fast32_t search_count       = 16;
constexpr uint_fast32_t astar_depth        = 10;

constexpr double_t occpancy_weight         = 50;
constexpr double_t is_on_decided_weight    = 20;
constexpr double_t is_angle_weight         = 2;
constexpr double_t is_side_weight          = 2;
constexpr double_t is_inside_closed_weight = 10;

constexpr uint_fast32_t max_mine_distance  = 20;
constexpr uint_fast32_t min_mine_distance  = 2;
constexpr uint_fast32_t min_agent_distance = 2;
constexpr uint_fast32_t min_goal_distance  = 2;
constexpr uint_fast32_t max_move           = 30;
constexpr uint_fast32_t min_move_cost      = 2;
constexpr int_fast32_t min_value          = 10;

constexpr double_t score_weight            = 1.1;
constexpr double_t goal_weight             = 8;
constexpr double_t cost_weight             = 0.023;

constexpr int_fast32_t min_open_list_value  = 8;
constexpr uint_fast32_t search_time         = 30000;
constexpr uint_fast32_t grace_time          = 2000;


/*
constexpr uint_fast32_t search_time         = 30000;
constexpr uint_fast32_t grace_time          = 2000;

constexpr uint_fast32_t max_mine_distance  = 20;
constexpr uint_fast32_t min_mine_distance  = 2;
constexpr uint_fast32_t astar_depth        = 10;

static int_fast32_t min_open_list_value = 1;
static uint_fast32_t greedy_count = 1;
static uint_fast32_t search_count = 1;

static double_t occpancy_weight = 1;
static double_t is_on_decided_weight = 1;
static double_t is_angle_weight = 1;
static double_t is_side_weight = 1;
static double_t is_inside_closed_weight = 1;

static uint_fast32_t min_agent_distance = 1;
static uint_fast32_t min_goal_distance = 1;
static uint_fast32_t max_move = 1;
static uint_fast32_t min_move_cost = 1;
static uint_fast32_t min_value = 1;

static double_t score_weight = 1;
static double_t goal_weight = 1;
static double_t cost_weight = 1;
*/

#define ANGLE_COORD 1
#define SIDE_COORD  2

class Astar : public AI{
private:
	friend void _multiThread(Astar* astar, Field field, const uint_fast32_t agent, std::pair<uint_fast32_t, uint_fast32_t> coord);
	int_fast32_t average_score;

	//探索法
	Random random;
	BeamSearch beam_search;
	BreadthForceSearch breadth_force_search;
	Greedy greedy;
	SimpleMove simple_move;

private:

	//探索かけるたびにクリアする
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> search_target;
	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> decided_route;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_goal;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_coord;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> next_coord;
	std::vector<int_fast32_t> current_score;


	//マルチスレッド用
	std::pair<uint_fast32_t, uint_fast32_t> tentative_goal;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> tentative_route;
	double tentative_max_score;

	
	//カウンター
	std::vector<uint_fast32_t> counter;

	
	//時間管理
	std::chrono::system_clock::time_point clock;
	bool is_time_over;

	
private:

	//移動
	void greedyMove(Field& field, const uint_fast32_t agent, const uint_fast32_t move_num);
	//void decidedMove(Field& field, const uint_fast32_t agent, std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>>& route);
	Direction exceptionMove(Field& field, const uint_fast32_t agent);
	Direction finalPhase(Field& field, const uint_fast32_t agent);

private:

	//ゴール候補の選定
	void setAverageScore(const Field& field);
	void setSearchTarget(Field& field, const uint_fast32_t agent);
	static const bool _comp(std::pair<double, std::pair<uint_fast32_t, uint_fast32_t>>& lhs ,std::pair<double, std::pair<uint_fast32_t, uint_fast32_t>>& rhs);
	const double goalEvaluation(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord);

	
	//ゴール選定用の評価関数関連
	const int_fast32_t occupancyRate(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t occupancyMineRate(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t whosePanel(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t isSideOrAngle(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	
	const bool isOnDecidedRoute(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool isMyPannel(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool isInsideClosed(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool isAngleCoord(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool isSideCoord(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	
	//ゴール候補選定
	const bool expectTarget(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool isOutOfRange(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool anotherAgentDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool anotherGoalDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;

	
	//評価関数 & 位置関係所得
	const double heuristic(const std::pair<uint_fast32_t, uint_fast32_t>& coord, const std::pair<uint_fast32_t, uint_fast32_t>& goal) const;
	
	const bool isAdjacentAgent(const Field& field, const uint_fast32_t agent, const uint_fast32_t attr);
	const bool isAdjacentMineAgent(const Field& field, const uint_fast32_t agent);
	const bool isAdjacentEnemyAgent(const Field& field, const uint_fast32_t agent);
	
	const uint_fast32_t countAdjacentAgent(const Field& field, const uint_fast32_t agent, const uint_fast32_t attr);
	const uint_fast32_t countAdjacentMineAgent(const Field& field, const uint_fast32_t agent);
	const uint_fast32_t countAdjacentEnemyAgent(const Field& field, const uint_fast32_t agent);
	
	const double averageDistanceAgent(const Field& field, const uint_fast32_t agent, const uint_fast32_t attr);
	const double averageDistanceMineAgent(const Field& field, const uint_fast32_t agent);
	const double averageDistanceEnemyAgent(const Field& field, const uint_fast32_t agent);
	
	const uint_fast32_t countWithinRangeAgent(const Field& field, const uint_fast32_t agent, const double range, const uint_fast32_t attr);
	const uint_fast32_t countWithinRangeMineAgent(const Field& field, const uint_fast32_t agent, const double range);
	const uint_fast32_t countWithinRangeEnemyAgent(const Field& field, const uint_fast32_t agent, const double range);


	//探索関連
	void initNode(const Field& field, std::vector<Node>& node);
	static const bool comp(std::pair<Node*, Field>& lhs, std::pair<Node*, Field>& rhs);

	
  std::pair<double, std::vector<Node>> searchRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, const uint_fast32_t max_move_cost);	
	void setStartNode(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* start);
	void setNextNode(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* current, Node* next);


	//枝切り用
	const bool branchingCondition(Field& field, const uint_fast32_t agent, Node* current, const uint_fast32_t max_move_cost);
	const bool endCondition(Node* current) const;
	const bool isPushOpenlist(Field& field, Node* next) const;

	
	//時間管理
	const bool isTimeOver() const;

	
	//マルチスレッド用
	void multiThread(Field field, const uint_fast32_t agent, std::pair<uint_fast32_t, uint_fast32_t> coord);


	//最適ルート検索
	void searchBestRoute(Field& field, const uint_fast32_t agent);
	void search(Field& field, const uint_fast32_t attr);
	const std::vector<std::pair<uint_fast32_t, uint_fast32_t>> makeRoute(Field& field, std::vector<Node>& node, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal);
	void setDecidedCoord(const std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route);

	
	//描画関連
	const void printGoal(Field& field, const uint_fast32_t attr) const;
	const void printRoute(std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route);


	//アルゴリズムの選択
	void chooseAlgorithm(Field& field, const uint_fast32_t agent);
	void singleMove(Field& field, const uint_fast32_t agent);
	void correctionRoute(Field& field, const uint_fast32_t agent);

public:

	Astar();
	~Astar();

	void init(const Field* field) override;
	void init(const Field& field) override;
	void move(Field *field, const uint_fast32_t attr) override;

};

//マルチスレッド用
static std::pair<uint_fast32_t, uint_fast32_t> _tentative_goal;
static std::vector<std::pair<uint_fast32_t, uint_fast32_t>> _tentative_route;
static double _tentative_max_score;

void _multiThread(Astar* astar, Field field, const uint_fast32_t agent, std::pair<uint_fast32_t, uint_fast32_t> coord);
