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
	
	const std::vector<int> vec_x={0,1,1,1,0,-1,-1,-1,0};
	const std::vector<int> vec_y={-1,-1,0,1,1,1,0,-1,0};
	
public:
	
	AI();
	~AI();
	virtual void move(Field *field, const uint_fast32_t attr) = 0;
	
};

class Greedy : public AI{
private:
	
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> decided_coord;
	
public:
	
	Greedy();
	~Greedy();
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void singleMove(Field& field, const uint_fast32_t agent);
	int_fast32_t nextScore(Field field, const uint_fast32_t agent, const Direction direction) const;
	void move(Field *field, const uint_fast32_t attr) override;
	
};

//各座標の状況を表すクラス
class Node{
private:
	
	static const uint_fast32_t move_weight      = 8;
	static const uint_fast32_t state_weight     = 1;
	static const uint_fast32_t heuristic_weight = 3;
	
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
  //親のノード
	Node* parent;
	
	//座標
	std::pair<uint_fast32_t, uint_fast32_t> coord;
	
	//スコア所得
	const double getScore() const;
	const double getHeuristic() const;
	
};

class Astar : public AI{
private:

	//描画用
	const uint_fast32_t cell_size  = 30;
	const uint_fast32_t point_size = 15;
	const uint_fast32_t half = cell_size / 2;
	
	//何個目まで探索
	static const uint_fast16_t search_depth = 8;
	
	int_fast32_t average_score;
	Greedy greedy;
	
private:

	//探索かけるたびにクリアする
	//各座標の状況を格納するリスト
	std::vector<std::vector<Node>> node;
	
	//探索対象リスト
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> search_target;	
	//確定ルートリスト
	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> decided_route;
	//確定方向リスト
	std::vector<std::vector<Direction>> decided_direction;

private:
	
	void decidedMove(Field& field, const uint_fast32_t agent);

private:
	
	//ゴール候補の選定
	void setAverageScore(const Field& field);
	void setSearchTarget(Field field, const uint_fast32_t agent);
	const double goalEvaluation(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& goal) const;
	//ゴール選定用の評価関数関連
	const bool expectTarget(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
  const bool isOnDecidedRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const bool anotherDistance(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t whosePanel(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;
	const uint_fast32_t occupancyRate(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t>& coord) const;

	//評価値関連
	const uint_fast32_t heuristic(const std::pair<uint_fast32_t, uint_fast32_t> coord, const std::pair<uint_fast32_t, uint_fast32_t> goal) const;
	//探索関連
	const double search(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> goal);
	void initNode(const Field& field);
	static const bool comp(std::pair<Node*, Field> lhs, std::pair<Node*, Field> rhs);

public:
	
	Astar();
	~Astar();
	
	void init(const Field& field);
	const void printGoal() const;
	const void printRoute(std::pair<uint_fast32_t, uint_fast32_t> goal) const;
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void move(Field *field, const uint_fast32_t attr) override;
	
};

class Random : public AI{
private:
	XorOshiro128p random;
public:
	Random();
	~Random();
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void move(Field *field, const uint_fast32_t attr) override;
};
