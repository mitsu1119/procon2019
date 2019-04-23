#include "include/agent.hpp"

Agent::Agent() {
}

Agent::Agent(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr):x(x),y(y),attr(attr) {
}

uint_fast32_t Agent::getX() {
	return this->x;
}

uint_fast32_t Agent::getY() {
	return this->y;
}

uint_fast32_t Agent::getAttr() {
	return this->attr;
}
