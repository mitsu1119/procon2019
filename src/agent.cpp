#include "include/agent.hpp"

Agent::Agent() {
}

Agent::Agent(unsigned int x, unsigned int y, unsigned int attr):x(x),y(y),attr(attr) {
}

unsigned int Agent::getX() {
	return this->x;
}

unsigned int Agent::getY() {
	return this->y;
}

unsigned int Agent::getAttr() {
	return this->attr;
}
