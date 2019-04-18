#include "include/field.hpp"

// ---------------------------------------- Panel ----------------------------------------

Panel::Panel():value(0) {
	setPure();
}

Panel::Panel(int value):value(value) {
	setPure();
}

void Panel::setValue(int value) {
	this->value = value;
}

bool Panel::isMyPanel() const {
	return this->attr & MINE_ATTR;
}

bool Panel::isEnemyPanel() const {
	return this->attr & ENEMY_ATTR;
}

bool Panel::isPurePanel() const {
	return this->attr & PURE_ATTR;
}

void Panel::setMine() {
	this->attr |= MINE_ATTR;
}

void Panel::setEnemy() {
	this->attr |= ENEMY_ATTR;
}

void Panel::setPure() {
	this->attr |= PURE_ATTR;
}

int Panel::getValue() const {
	return this->value;
}

// ---------------------------------------- Field ---------------------------------------------

Field::Field(unsigned int width, unsigned int height):width(width),height(height) {
	double buf;
	unsigned int size;

	buf = std::log2(this->width);
	this->yShiftOffset = (unsigned int)(buf + ((std::ceil(buf) == std::floor(buf)) ? 0 : 1));
	size = this->height << this->yShiftOffset;

	this->field = std::vector<Panel>(size, Panel(0));	
}

const Panel *Field::at(unsigned int x, unsigned int y) const {
	return (const Panel *)&(this->field[x + (y << this->yShiftOffset)]);
}

void Field::print() {
	for(size_t i = 0; i < this->height; i++) {
		for(size_t j = 0; j < this->width; j++) {
			printf("%2d ", this->at(j, i)->getValue());
		}
		printf("\n");
	}
}
