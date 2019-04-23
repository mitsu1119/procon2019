#include "include/field.hpp"

// ---------------------------------------- Panel ----------------------------------------

Panel::Panel():value(0) {
	setPure();
}

Panel::Panel(int_fast32_t value):value(value) {
	setPure();
}

void Panel::setValue(int_fast32_t value) {
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

int_fast32_t Panel::getValue() const {
	return this->value;
}

// ---------------------------------------- Field ---------------------------------------------

Field::Field(uint_fast32_t width, uint_fast32_t height):width(width),height(height) {
	double buf;
	uint_fast32_t size;

	this->random = XorOshiro128p(time(NULL));

	buf = std::log2(this->width);
	this->yShiftOffset = (uint_fast32_t)(buf + ((std::ceil(buf) == std::floor(buf)) ? 0 : 1));
	size = this->height << this->yShiftOffset;

	// マップ生成
	this->field = std::vector<Panel>(size, Panel(0));	
	genRandMap();

	// とりあえずAgentを適当に生成
	this->agents.emplace_back(0, 0, MINE_ATTR);
	this->agents.emplace_back(width - 1, 0, ENEMY_ATTR);
	this->agents.emplace_back(0, height - 1, ENEMY_ATTR);
	this->agents.emplace_back(width - 1, height - 1, MINE_ATTR);
}

void Field::setPanelScore(uint_fast32_t x, uint_fast32_t y, int_fast32_t value) {
	this->field[x + (y << this->yShiftOffset)].setValue(value);
}

void Field::genRandMap() {
	int_fast32_t buf;
	for(size_t i = 0; i < height; i++) {
		for(size_t j = 0; j < width; j++) {
			buf = (int)this->random.randull(32) - 16;
			setPanelScore(j, i, buf);
		}
	}
}

const Panel *Field::at(uint_fast32_t x, uint_fast32_t y) const {
	return (const Panel *)&(this->field[x + (y << this->yShiftOffset)]);
}

void Field::print() {
	uint_fast32_t flag;

	for(size_t i = 0; i < this->height; i++) {
		for(size_t j = 0; j < this->width; j++) {
			// エージェントの表示処理
			// ||agents|| が十分に小さいため線形探索でも計算時間にそれほど影響がでない
			flag = PURE_ATTR;
			for(auto &a: this->agents) {
				if(a.getX() == j && a.getY() == i) {
					flag = a.getAttr();
					break;
				}
			}
			if(flag == MINE_ATTR) printf("\x1b[36m");
			if(flag == ENEMY_ATTR) printf("\x1b[31m");
			printf("%3d ", this->at(j, i)->getValue());
			if(flag != PURE_ATTR) printf("\x1b[39m");
		}
		printf("\n");
	}
}
