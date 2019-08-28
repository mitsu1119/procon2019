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
	virtual void move(Field *field, const uint_fast32_t attr) = 0;
	virtual	void init(const Field* field) = 0;
	virtual	void init(const Field& field) = 0;

	
};

class Random : public AI{
private:
	
	XorOshiro128p random;
	
public:
	
	Random();
	~Random();

	void init(const Field* field) override;
	void init(const Field& field) override;
	
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void move(Field *field, const uint_fast32_t attr) override;
	
};

class Greedy : public AI{
private:

	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_coord;
	XorOshiro128p random;
	uint_fast32_t current_score;
	
public:
	
	Greedy();
	~Greedy();

	void init(const Field* field) override;
	void init(const Field& field) override;
	
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void singleMove(Field& field, const uint_fast32_t agent);
	void randomMove(Field& field, const uint_fast32_t agent, const uint_fast32_t x, const uint_fast32_t y);
	int_fast32_t nextScore(Field field, const uint_fast32_t agent, const Direction direction) const;
	void move(Field *field, const uint_fast32_t attr) override;
	
};

class BeamSearch : public AI{
private:
	
public:
	
	BeamSearch();
	~BeamSearch();

	void init(const Field* field) override;
	void init(const Field& field) override;
	
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void move(Field *field, const uint_fast32_t attr) override;
	
};

class BreadthForceSearch : public AI{
private:
	
public:

	BreadthForceSearch();
	~BreadthForceSearch();

	void init(const Field* field) override;
	void init(const Field& field) override;
	
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void move(Field *field, const uint_fast32_t attr) override;
	
};

constexpr uint_fast32_t move_weight                = 9;
constexpr uint_fast32_t state_weight               = 4;
constexpr uint_fast32_t heuristic_weight           = 3.6;
constexpr uint_fast32_t value_weight               = 7;
constexpr uint_fast32_t is_on_decided_route_weight = 12;
constexpr uint_fast32_t is_on_mine_panel_weight    = 4;

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
	//const double getHeuristic() const;
	
};

/*
inline const double Node::getHeuristic() const{
	return this->heuristic * heuristic_weight;
}
*/

inline const double Node::getScore() const{
	return ((this->move_cost * move_weight) + (this->state_cost * state_weight) + (this->heuristic * heuristic_weight) + (this->is_on_decided_route * is_on_decided_route_weight) + (this->value * value_weight)) + (this->is_on_mine_panel * is_on_mine_panel_weight);
}

constexpr uint_fast32_t occpancy_weight = 2;

constexpr uint_fast16_t search_depth = 5;

class Astar : public AI{
private:

	int_fast32_t average_score;
	Greedy greedy;
	BeamSearch beam_search;
	
private:

	//探索かけるたびにクリアする
	std::vector<Node> node;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> search_target;
	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> decided_route;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_goal;
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_coord;

	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> next_coord;
	
private:

	//貪欲での移動
	void greedyMove(Field& field, const uint_fast32_t agent, const uint_fast32_t move_num);

	
	//探索内での確定移動
	void decidedMove(Field& field, const uint_fast32_t agent, std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>>& route);
	const Direction changeDirection(const std::pair<uint_fast32_t, uint_fast32_t>& now, const std::pair<uint_fast32_t, uint_fast32_t>& next) const;

private:

	//ゴール候補の選定
	void setAverageScore(const Field& field);
	void setSearchTarget(Field& field, const uint_fast32_t agent);
	const double goalEvaluation(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal);

	
	//ゴール選定用の評価関数関連
	const uint_fast32_t occupancyRate(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t isSideOrAngle(Field& field, const std::pair<uint_fast32_t, uint_fast32_t>& coord);

	
	const bool expectTarget(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord);
  const bool isOnDecidedRoute(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool anotherAgentDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool anotherGoalDistance(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t whosePanel(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;

	
	//評価値関連
	const uint_fast32_t heuristic(const std::pair<uint_fast32_t, uint_fast32_t>& coord, const std::pair<uint_fast32_t, uint_fast32_t>& goal) const;

	
	//探索関連
	void initNode(const Field& field);
	static const bool comp(std::pair<Node*, Field>& lhs, std::pair<Node*, Field>& rhs);

	
	const double searchRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal);
	void setStartNode(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* start);
	void setNextNode(Field& next_field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal, Node* current, Node* next);

	
	const bool branchingCondition(Node* current) const;
	const bool endCondition(Node* current) const;
	
	
	void searchBestRoute(Field& field, const uint_fast32_t agent);
	void search(Field& field, const uint_fast32_t attr);
	const std::vector<std::pair<uint_fast32_t, uint_fast32_t>> makeRoute(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal);

	
	//描画関連
	const void printGoal(Field& field, const uint_fast32_t attr) const;
	const void printRoute(std::vector<std::pair<uint_fast32_t, uint_fast32_t>> route);

	
	//アルゴリズムの選択
	void chooseAlgorithm(Field& field, const uint_fast32_t agent);

	//着手
	void singleMove(Field& field, const uint_fast32_t agent);
	
	
public:
	
	Astar();
	~Astar();

	void init(const Field* field) override;
	void init(const Field& field) override;
	
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void move(Field *field, const uint_fast32_t attr) override;
	
};

inline const Direction Astar::changeDirection(const std::pair<uint_fast32_t, uint_fast32_t>& now, const std::pair<uint_fast32_t, uint_fast32_t>& next) const{
	for(size_t i = 0; i < DIRECTION_SIZE - 2; i++)
		if(next.first == now.first + this->vec_x.at(i) && next.second == now.second + this->vec_y.at(i))
			return (Direction)i;
}

