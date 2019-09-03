#include "agent.hpp"

Agent::Agent() {
}

Agent::Agent(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr):x(x),y(y),attr(attr) {
}

Agent::Agent(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr, uint_fast32_t id):x(x),y(y),attr(attr),agent_id(id) {
}

void Agent::move(Direction direction) {
	this->nextX = this->x + direction2x(direction);
	this->nextY = this->y + direction2y(direction);
}

uint_fast32_t Agent::getX() {
	return this->x;
}

uint_fast32_t Agent::getY() {
	return this->y;
}

uint_fast32_t Agent::getnextX() {
	return this->nextX;
}

uint_fast32_t Agent::getnextY() {
	return this->nextY;
}

uint_fast32_t Agent::getAttr() {
	return this->attr;
}

void Agent::setNext() {
	this->x = this->nextX;
	this->y = this->nextY;
}

void Agent::reverseAttr(){
	this->attr = this->attr ^ 0b11;
}
