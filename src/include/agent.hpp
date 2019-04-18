#pragma once

#include "field.hpp"

class Agent {
private:
	struct {
		// x座標, y座標
		// 0 <= x, y < 20 
		unsigned int x: 5;
		unsigned int y: 5;

		// 属性。Panelの属性のMINE_ATTRとENEMY_ATTRをそのまま使う
		unsigned int attr: 2;
	};

public:
	Agent();
	Agent(unsigned int x, unsigned int y, unsigned int attr);

	unsigned int getX();
	unsigned int getY();
	unsigned int getAttr();
};

