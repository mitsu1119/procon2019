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

		// 次に移動する座標のバッファ(全探索用)
		uint_fast32_t nextbufX: 5;
		uint_fast32_t nextbufY: 5;
		
	};
	
	uint_fast32_t agent_id;

public:
	Agent();
	Agent(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr);
	Agent(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr, uint_fast32_t id);

	void move(Direction direction);

	uint_fast32_t getX();
	uint_fast32_t getY();
	uint_fast32_t getnextX();
	uint_fast32_t getnextY();
	void setnextbufXY(uint_fast32_t nextbufX, uint_fast32_t nextbufY);
	uint_fast32_t getnextbufX() const;
	uint_fast32_t getnextbufY() const;
	uint_fast32_t getAttr();

	uint_fast32_t getID();

	// (nextX, nextY)を(x, y)に適用する
	void setNext();
	void reverseAttr();
};

