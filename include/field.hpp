#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <climits>
#include "agent.hpp"
#include "useful.hpp"
#include "disp.hpp"

constexpr uint_fast32_t MINE_ATTR = 0b01;
constexpr uint_fast32_t ENEMY_ATTR = 0b10;
constexpr uint_fast32_t PURE_ATTR = 0b00;

class Field;
class Agent;
class Display;
class DisplayWrapper;

class MoveLog {
private:
	
	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> coord_log;
	//確定ルート所得で使う
	std::vector<std::vector<Direction>> direction_log;
	
public:
	
	MoveLog();
	~MoveLog();
	void init(const Field& field);
	void set(const uint_fast32_t agent, std::pair<uint_fast32_t, uint_fast32_t> coord);
	void revert(Field& field);
	bool repetitionOfMoves(const uint_fast32_t agent, const Direction direction) const;
	
};

/*
 * Panel 一つ一つの情報を管理するクラス
 */
class Panel {
	friend Field;
private:
	struct {
		// パネルのスコア
		// -16 <= value <= 16
		int_fast32_t value: 6;

		// パネルの属性を格納
		// 01:味方パネル 10:敵パネル 00:未占領 11:エラー
		// XXX 支配者 XXX
		uint_fast32_t attr: 2;
	};

	// パネルにスコアを代入
	void setValue(int_fast32_t value);	

public:
	Panel();
	Panel(int_fast32_t value);

	// 持ち主は誰か
	// ex. isMyPanel() == true => 味方パネル
	bool isMyPanel() const;
	bool isEnemyPanel() const;
	bool isPurePanel() const;
	uint_fast32_t getAttr() const;

	// 持ち主のセット
	// ex. setMine() -> 味方パネルへ
	void setMine();
	void setEnemy();
	void setPure();

	// パネルのスコアを所得
	int_fast32_t getValue() const;
};	

/*
 * Field 一つ一つ
 */
class Field {
	
	friend DisplayWrapper;
	friend Display;
	
private:

	// フィールドの実幅、実高さ
	uint_fast32_t width, height;

	// 次の行に行くために必要なyのシフト数
	uint_fast32_t yShiftOffset;

	// 乱数生成器
	XorOshiro128p random;

	std::vector<Panel> field;
	//	std::vector<Agent> agents;	

	// agentの動作可能かベクター
	std::vector<bool> canmoveAgents;

	// 実座標からfieldのインデックスを取り出す関数
	uint_fast32_t xyIndex(uint_fast32_t x, uint_fast32_t y);

	// fieldのインデックスから実座標を所得
	uint_fast32_t indexY(uint_fast32_t index) {
		return index / (field.size() / height);
	}
	uint_fast32_t indexX(uint_fast32_t index) {
		return index - indexY(index) * (field.size() / height);
	}

	// 実座標(x,y)のパネルにスコアをセット
	void setPanelScore(uint_fast32_t x, uint_fast32_t y, int_fast32_t value);

	// 実座標(x,y)のパネルの属性をセット
	void setPanelAttr(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr);

	// フィールドランダム生成用関数
	//void genRandMap();

	// agentがdirectionの方向に動けるかどうか true:動ける false:動けない
	// bool canMove(Agent &agent, Direction direction);

	// agentsの(nextX,nextY)を適用する
	// 行きたい座標が重なるなどしたときその場に止まるように指示
	// void applyNextAgents();

	// 得点計算関係
	UF makePureTreeMine();
	UF makePureTreeEnemy();
	std::unordered_map<int_fast32_t , std::vector<int_fast32_t>> makePureTerritory(UF &&pureTree);
	bool isPanelMineBetween(uint_fast32_t x, uint_fast32_t y);
	bool isPanelEnemyBetween(uint_fast32_t x, uint_fast32_t y);
	bool checkLocalArea(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr);
	int_fast32_t calcMineScore(std::unordered_map<int_fast32_t , std::vector<int_fast32_t>> &pureTree);
	int_fast32_t calcEnemyScore(std::unordered_map<int_fast32_t, std::vector<int_fast32_t>> &pureTree);

public:

	void genRandMap();
	std::vector<Agent> agents;
	
	// agentがdirectionの方向に動けるかどうか true:動ける false:動けない
	bool canMove(Agent &agent, Direction direction);

	// agentsの(nextX,nextY)を適用する
	// 行きたい座標が重なるなどしたときその場に止まるように指示
	void applyNextAgents();
	
	Field(uint_fast32_t width, uint_fast32_t height);
	// MINE, ENEMY属性のパネルの置かれているところの合計点数を所得
	int_fast32_t calcMinepanelScore();
	int_fast32_t calcEnemypanelScore();

	// スコア計算m
	int_fast32_t calcScore(uint_fast32_t attr);

	// 実座標(x,y)のパネルのポインタを所得(const)
	const Panel *at(uint_fast32_t x, uint_fast32_t y) const;

	// テスト用. ランダムな方向へエージェントが移動する
	void testMoveAgent();

	// コンソール上に表示(テスト用)
	void print();

  //width、heightを外部から所得
	const uint_fast32_t getWidth() const;
	const uint_fast32_t getHeight() const;

	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> decided_route;
	
};

