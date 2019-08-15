#pragma once

#include "agent.hpp"
#include "field.hpp"
#include "useful.hpp"

class Agent;
class Field;

class AI{
private:
public:
	AI();
	~AI();
	virtual void move(Field *field, uint_fast32_t attr) = 0;
};

class Greedy : public AI{
private:
	
	std::vector<Direction> decided_direction;
	
public:
	
	Greedy();
	~Greedy();
	void init(const Field& field);
	//指定したエージェントを貪欲法で動かす
	void mineMove(Field& field);
	void enemyMove(Field& field);
	void singleMove(Field& field, const uint_fast32_t agent);
	int_fast32_t nextScore(Field field, const uint_fast32_t agent, Direction direction) const;
	void decidedMove(Field& field, uint_fast32_t attr) const;
	//エージェントの移動
	void move(Field *field, uint_fast32_t attr) override;
	
};

enum Status{
NONE,
OPEN,
CLOSED
};

//各座標の状況を表すクラス
class Node{
private:
public:
	
	Node();
	~Node();
	//ステータス
	Status status;
	//移動コスト
	uint_fast32_t moveCost;
	//盤の状態によるコスト
	double stateCost;
	//ヒューリスティックコスト（推定コスト）
	uint_fast32_t heuristic;
  //親のノード
	Node* parent;
	//座標
	std::pair<uint_fast32_t, uint_fast32_t> coord;
	//スコア所得
	const double getScore() const;
	
};

class Astar : public AI{
private:
	
	//何個目まで探索をかけるか？
	static const uint_fast16_t search_depth = 15;
	//重さ
	static const uint_fast32_t heuristic_weight = 3;
	static const uint_fast32_t score_weight = 3;
	//盤の平均値
	int_fast32_t average_score;
	//貪欲法
	Greedy greedy;
	
private:
	
	//探索かけるたびにクリアする
	//各座標の状況を格納するリスト
	std::vector<std::vector<Node>> node;
	//ノードと対にして予測盤情報を格納するリスト
	std::vector<std::vector<Field>> forecast_field;
	
	//ゴールの候補リスト
	//std::vector<std::pair<uint_fast32_t, uint_fast32_t>> goal_condidate_list;
	
	//探索対象リスト
	std::vector<std::pair<uint_fast32_t, uint_fast32_t>> search_target_list;
	
	//確定ルートリスト
	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> decided_route;
	//確定方向リスト
	std::vector<std::vector<Direction>> decided_direction;

private:
	
	//指定したエージェントを確定方向で動かす	
	void decidedMove(Field& field, const uint_fast32_t agent);

private:
	
	//ゴール候補の選定
	void setAverageScore(const Field& field);
	void setSearchTargetList(Field field, const uint_fast32_t agent);
	int_fast32_t selectGoalCondidate(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> coord);
	bool isDecidedRoute(Field field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> coord);

	
	//void setGoalCondidateList(const Field& field);
	//void sortSearchTargetList(Field field, const uint_fast32_t agent);

		//ヒューリスティックコスト（推定コスト）の計算
	const uint_fast32_t heuristicCost(const std::pair<uint_fast32_t, uint_fast32_t> coord, const std::pair<uint_fast32_t, uint_fast32_t> goal) const;
	//A*による推定移動コスト
	const double estimeteMoveCost(Field& field, const uint_fast32_t agent, const std::pair<uint_fast32_t, uint_fast32_t> goal) const;
	
public:
	
	Astar();
	~Astar();
	
	void init(const Field& field);
	void move(Field *field, uint_fast32_t attr) override;
	void print() const;
	
};

class Random : public AI{
private:
public:
	Random();
	~Random();
	void mineMove(Field* field);
	void enemyMove(Field* field);
	void move(Field *field, uint_fast32_t attr) override;
};
