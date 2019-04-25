#pragma once

#include "field.hpp"
#include "useful.hpp"

class Agent {
private:
	struct {
		// x座標, y座標
		// 0 <= x, y < 20 
		uint_fast32_t x: 5;
		uint_fast32_t y: 5;

		// 属性。Panelの属性のMINE_ATTRとENEMY_ATTRをそのまま使う
		uint_fast32_t attr: 2;

		// 次に移動する座標
		uint_fast32_t nextX: 5;
		uint_fast32_t nextY: 5;
	};

public:
	Agent();
	Agent(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr);

	void move(Direction direction);

	uint_fast32_t getX();
	uint_fast32_t getY();
	uint_fast32_t getnextX();
	uint_fast32_t getnextY();
	uint_fast32_t getAttr();

	// (nextX, nextY)を(x, y)に適用する
	void setNext();
};

