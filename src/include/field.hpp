#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "agent.hpp"
#include "useful.hpp"

constexpr unsigned int MINE_ATTR = 0b01;
constexpr unsigned int ENEMY_ATTR = 0b10;
constexpr unsigned int PURE_ATTR = 0b00;

class Field;

/*
 * Panel 一つ一つの情報を管理するクラス
 */
class Panel {
friend Field;
private:
	struct {
		// パネルのスコア
		// -16 <= value <= 16
		int value: 6;

		// パネルの属性を格納
		// 01:味方パネル 10:敵パネル 00:未占領 11:エラー
		// XXX 支配者 XXX
		unsigned int attr: 2;
	};

	// パネルにスコアを代入
	void setValue(int value);	

public:
	Panel();
	Panel(int value);

	// 持ち主は誰か
	// ex. isMyPanel() == true => 味方パネル
	bool isMyPanel() const;
	bool isEnemyPanel() const;
	bool isPurePanel() const;

	// 持ち主のセット
	// ex. setMine() -> 味方パネルへ
	void setMine();
	void setEnemy();
	void setPure();

	// パネルのスコアを所得
	int getValue() const;
};	

class Agent;

/*
 * Field 一つ一つ
 */
class Field {
private:
	// フィールドの実幅、実高さ
	unsigned int width, height;

	// 次の行に行くために必要なyのシフト数
	unsigned int yShiftOffset;

	// 乱数生成器
	XorOshiro128p random;

	std::vector<Panel> field;
	std::vector<Agent> agents;

	// 実座標(x,y)のパネルにスコアをセット
	void setPanelScore(unsigned int x, unsigned int y, int value);

	// フィールドランダム生成用関数
	void genRandMap();

public:
	Field(unsigned int width, unsigned int height);

	// 実座標(x,y)のパネルのポインタを所得(const)
	const Panel *at(unsigned int x, unsigned int y) const;

	// コンソール上に表示(テスト用)
	void print();
};
