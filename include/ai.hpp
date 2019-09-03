#pragma once

#include "agent.hpp"
#include "field.hpp"
#include "useful.hpp"
#include <thread>

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

	static const bool MineComp(std::pair<Field, Field>& lhs, std::pair<Field, Field>& rhs);
	static const bool EnemyComp(std::pair<Field, Field>& lhs, std::pair<Field, Field>& rhs);

	
};

inline const Direction AI::changeDirection(const std::pair<uint_fast32_t, uint_fast32_t>& now, const std::pair<uint_fast32_t, uint_fast32_t>& next) const{
	for(size_t i = 0; i < DIRECTION_SIZE - 2; i++)
		if(next.first == now.first + this->vec_x.at(i) && next.second == now.second + this->vec_y.at(i))
			return (Direction)i;
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

	std::mutex mtx;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_coord;
	XorOshiro128p random;

private:
	
	int_fast32_t nextScore(Field field, const uint_fast32_t agent, const Direction direction) const;
	
public:
	
	Greedy();
	~Greedy();

	void init(const Field* field) override;
	void init(const Field& field) override;
	
	void singleMove(Field& field, const uint_fast32_t agent);
	void randomMove(Field& field, const uint_fast32_t agent, const uint_fast32_t x, const uint_fast32_t y);
	
	void move(Field *field, const uint_fast32_t attr) override;
	
};

constexpr uint_fast32_t beam_depth = 3;
constexpr uint_fast32_t beam_width = 3;

class BeamSearch : public AI{

private:

	Greedy greedy;
	
	Field search(Field* field, const uint_fast32_t agent, uint_fast32_t depth);
	
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


constexpr double_t move_weight                = 3;
constexpr double_t state_weight               = 40;
constexpr double_t heuristic_weight           = 4;
constexpr double_t value_weight               = 15;
constexpr double_t is_on_decided_route_weight = 15;
constexpr double_t is_on_mine_panel_weight    = 4;

class Node{
private:

public:

	enum Status{
		NONE,
		OPEN,
		CLOSED
	};
	
	Node();
	~Node();
	//ステータス
	Status status;
	//移動コスト
	uint_fast32_t move_cost;
	//盤の状態によるコスト
	double state_cost;
	//ヒューリスティックコスト（推定コスト）
	uint_fast32_t heuristic;
	//その地点での点数
	int_fast32_t  value;
	//確定ルートにかぶっているか？
  uint_fast32_t is_on_decided_route;	
	//自陣を何回移動したか
	uint_fast32_t is_on_mine_panel;
	//何回移動したか
	uint_fast32_t move_num;
  //親のノード
	Node* parent;	
	//座標
	std::pair<uint_fast32_t, uint_fast32_t> coord;
	
	//スコア所得
	const double getScore() const;
	
};

inline const double Node::getScore() const{
	return ((this->move_cost * move_weight) + (this->state_cost * state_weight) + (this->heuristic * heuristic_weight) + (this->is_on_decided_route * is_on_decided_route_weight) - (this->value * this->move_num * value_weight)) + (this->is_on_mine_panel * is_on_mine_panel_weight);
}


constexpr double greedy_count              = 3;
constexpr double occpancy_weight           = 2;
constexpr double is_on_decided_weight      = 10;

//ゴールの除外条件
constexpr uint_fast32_t max_mine_distance  = 20;
constexpr uint_fast32_t min_mine_distance  = 2;
//Agent同士の距離
constexpr uint_fast32_t min_agent_distance = 2;
//ゴール候補同士の距離
constexpr uint_fast32_t min_goal_distance  = 2;
//枝きり条件
constexpr uint_fast32_t max_move_cost      = 35;
constexpr uint_fast32_t min_value          = -5;
//終了条件
constexpr uint_fast32_t min_move_cost      = 2;


class Astar : public AI{
private:

	std::mutex mtx;
	int_fast32_t average_score;

	Random random;
	Greedy greedy;
	BeamSearch beam_search;
	BreadthForceSearch breadth_force_search;
	
private:

	//探索かけるたびにクリアする
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> search_target;
	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> decided_route;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_goal;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_coord;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> next_coord;

	
	//マルチスレッド用
	std::pair<uint_fast32_t, uint_fast32_t> tentative_goal;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> tentative_route;
	int_fast32_t tentative_max_score;

	
	
private:


	//移動
	void greedyMove(Field& field, const uint_fast32_t agent, const uint_fast32_t move_num);
	void decidedMove(Field& field, const uint_fast32_t agent, std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>>& route);
	

private:

	//ゴール候補の選定
	void setAverageScore(const Field& field);
	void setSearchTarget(Field& field, const uint_fast32_t agent);
	const double goalEvaluation(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal);

	
	//ゴール選定用の評価関数関連
	const uint_fast32_t occupancyRate(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t isSideOrAngle(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord);


	//ゴール候補の選定
	const bool expectTarget(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord);
  const bool isOnDecidedRoute(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool anotherAgentDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool anotherGoalDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t whosePanel(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;


	//ヒューリスティックコスト（推定コスト）計算
	const double heuristic(const std::pair<uint_fast32_t, uint_fast32_t>& coord, const std::pair<uint_fast32_t, uint_fast32_t>& goal) const;

	
	//探索関連
	void initNode(const Field& field, std::vector<Node>& node);
	static const bool comp(std::pair<Node*, Field>& lhs, std::pair<Node*, Field>& rhs);
	const std::tuple<int_fast32_t, std::vector<Node>, std::pair<uint_fast32_t, uint_fast32_t>> searchRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal);
	void setStartNode(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* start);
	void setNextNode(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* current, Node* next);


	//枝切り用
	const bool branchingCondition(Node* current) const;
	const bool endCondition(Node* current) const;
	

	//マルチスレッド用
	void multiThread(Field& field, const uint_fast32_t agent, std::pair<uint_fast32_t, uint_fast32_t> coord);

	
	void searchBestRoute(Field& field, const uint_fast32_t agent);
	void search(Field& field, const uint_fast32_t attr);
	

	const std::vector<std::pair<uint_fast32_t, uint_fast32_t>> makeRoute(Field& field, std::vector<Node>& node, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal);

	
	//描画関連
	const void printGoal(Field& field, const uint_fast32_t attr) const;
	const void printRoute(std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route);

	
	//アルゴリズムの選択
	void chooseAlgorithm(Field& field, const uint_fast32_t agent);
	
	
public:
	
	Astar();
	~Astar();

	void singleMove(Field& field, const uint_fast32_t agent);
	
	void init(const Field* field) override;
	void init(const Field& field) override;
	void move(Field *field, const uint_fast32_t attr) override;
	
};

