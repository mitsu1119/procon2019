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
	virtual void move(Field *field)=0;
};

//各座標の状況を表すクラス
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

class AstarMine : public AI{
private:
	
	//何個目まで探索をかけるか？
	static const uint_fast16_t search_depth = 5;
	//盤の平均・最大・最小値
	static int_fast32_t average_score;
	static int_fast32_t max_score;
	static int_fast32_t min_score;
	
private:
	
	//探索かけるたびにクリアする
	//各座標の状況を格納するリスト
	std::vector<std::vector<Node>> node;
	//ノードと対にして予測盤情報を格納するリスト
	std::vector<std::vector<Field>> forecast_field;
	//ゴールの候補リスト
	std::vector<std::pair<int_fast32_t, int_fast32_t>> goal_condidate_list;
	//探索対象リスト
	std::vector<std::pair<int_fast32_t, int_fast32_t>> search_target_list;	
	//確定ルートリスト
	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> decided_route;
	//確定方向リスト
	std::vector<std::vector<Direction>> decided_direction;

private:
	
	//指定したエージェントを貪欲法で動かす
	void greedyMove(Field& field, const uint_fast32_t agent);
	//指定したエージェントを確定方向で動かす	
	void decidedMove(Field& field, const uint_fast32_t agent);

private:

	void setParameters(const Field field);
	//ヒューリスティックコスト（推定コスト）の計算
	const uint_fast32_t heuristicCost(const std::pair<uint_fast32_t, uint_fast32_t> coord, const std::pair<uint_fast32_t, uint_fast32_t> goal) const;
	//探索順番にゴール候補を探索候補リストにソートする
	void sortSearchTargetList(const Field field, const uint_fast32_t agent);
	//A*による推定移動コスト
	const double estimeteMoveCost(Field& field, const uint_fast32_t agent) const;
	
public:
	
	AstarMine();
	~AstarMine();
	
	//初期化
	void init(const Field field);
	//エージェントの移動
	void move(Field *field) override;
	//表示
	void print() const;
	
};

class AstarEnemy : public AI{
private:
public:
	AstarEnemy();
	~AstarEnemy();
	void move(Field *field) override;
};

class RandomMine : public AI{
private:
public:
	RandomMine();
	~RandomMine();
	void move(Field *field) override;
};

class RandomEnemy : public AI{
private:
public:
	RandomEnemy();
	~RandomEnemy();
	void move(Field *field) override;
};
