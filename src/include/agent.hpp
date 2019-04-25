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
	};

public:
	Agent();
	Agent(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr);

	void move(Direction direction);

	uint_fast32_t getX();
	uint_fast32_t getY();
	uint_fast32_t getAttr();
};

