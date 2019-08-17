#include "field.hpp"

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
	return !this->attr;
}

uint_fast32_t Panel::getAttr() const {
	return this->attr;
}

void Panel::setMine() {
	this->attr |= MINE_ATTR;
}

void Panel::setEnemy() {
	this->attr |= ENEMY_ATTR;
}

void Panel::setPure() {
	this->attr = PURE_ATTR;
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
	this->agents.emplace_back(1, 1, MINE_ATTR);
	this->agents.emplace_back(width - 2, 1, ENEMY_ATTR);
	this->agents.emplace_back(1, height - 2, ENEMY_ATTR);
	this->agents.emplace_back(width - 2, height - 2, MINE_ATTR);

	this->agents.emplace_back(2, 2, ENEMY_ATTR);
	this->agents.emplace_back(width - 3, 2, MINE_ATTR);
	this->agents.emplace_back(2, height - 3, MINE_ATTR);
	this->agents.emplace_back(width - 3, height - 3, ENEMY_ATTR);

	this->canmoveAgents = std::vector<bool>(this->agents.size(), true);

	// エージェントの初期位置のパネルの属性を設定
	for(auto &i: this->agents) {
		setPanelAttr(i.getX(), i.getY(), i.getAttr());
	}

	//とりあえずSTOPにセット
	std::for_each(this->agents.begin(), this->agents.end(), [&, this](auto& a){
			a.move(STOP);
		});
}

uint_fast32_t Field::xyIndex(uint_fast32_t x, uint_fast32_t y) {
	return x + (y << this->yShiftOffset);
}

void Field::setPanelScore(uint_fast32_t x, uint_fast32_t y, int_fast32_t value) {
	this->field[x + (y << this->yShiftOffset)].setValue(value);
}

void Field::setPanelAttr(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr) {
	switch(attr) {
	case MINE_ATTR:
		this->field[x + (y << this->yShiftOffset)].setMine();
		break;
	case ENEMY_ATTR:
		this->field[x + (y << this->yShiftOffset)].setEnemy();
		break;
	case PURE_ATTR:
		this->field[x + (y << this->yShiftOffset)].setPure();
		break;
	default:
		fprintf(stderr, "Attr %x は存在しません。\n", attr);
		break;
	}
}

void Field::genRandMap() {
	int N = 32;
	std::vector<int> field_rand;
  std::vector<int> field_rev;
  std::vector<int> buf;
	int buf_height, buf_width;
	buf_height=(this->height/2)+(this->height%2);
	buf_width=(this->width/2)+(this->width%2);
	
	for(int i=0; i < buf_height; i++){
		for(int j=0; j < buf_width; j++){
			buf.push_back((rand() % N)-16);
		}
		field_rand.insert(field_rand.end(), buf.begin(), buf.end());
		std::reverse(buf.begin(), buf.end());
		field_rand.insert(field_rand.end(), buf.begin(), buf.end());
		buf.clear();
  }

  field_rev = field_rand;
  std::reverse(field_rev.begin(), field_rev.end());
  field_rand.insert(field_rand.begin(), field_rev.begin(), field_rev.end());

	//奇数業に対応させる

	if(this->height%2){
		field_rand.erase(field_rand.begin()+(field_rand.size()/2), field_rand.begin()+(field_rand.size()/2)+this->height+1);
	}
	if(this->width%2){
		for(int i=0;i<field_rand.size();i++){
			if(i%this->width==this->width/2)
				field_rand.erase(field_rand.begin()+i);
		}
	}
	
	int val=0;
	
	for(int i=0;i<this->height;i++){
		for(int j=0;j<this->width;j++){
			this->setPanelScore(j, i, field_rand.at(val));
			val++;
		}
	}
}

bool Field::canMove(Agent &agent, Direction direction) {
	int_fast32_t nextX = agent.getX() + direction2x(direction), nextY = agent.getY() + direction2y(direction);
	
	if(nextX < 0 || nextX > this->width - 1 || nextY < 0 || nextY > this->height - 1) return false;
	for(auto &i: this->agents) {
		if(i.getX() == nextX && i.getY() == nextY) return false;
	}

	return true;
}

void Field::applyNextAgents() {
	// ||agents||が小さいため愚直な実装でも問題なさそう
	// TODO:早くできるいい方法があるなら実装すべき	
	for(size_t i = 0; i < this->agents.size(); i++) {
		for(size_t j = i + 1; j < this->agents.size(); j++) {
			if(this->agents[i].getnextX() != this->agents[j].getnextX() ||  this->agents[i].getnextY() != this->agents[j].getnextY()) continue;
			this->canmoveAgents[i] = false;
			this->canmoveAgents[j] = false;
		}
	}

	for(size_t i = 0; i < this->agents.size(); i++) {
		if(this->canmoveAgents[i]) {
			if(!this->at(this->agents[i].getnextX(), this->agents[i].getnextY())->isPurePanel() && this->at(this->agents[i].getnextX(), this->agents[i].getnextY())->getAttr() != this->agents[i].getAttr()) {
				setPanelAttr(this->agents[i].getnextX(), this->agents[i].getnextY(), PURE_ATTR);
			} else {
				this->agents[i].setNext();
				setPanelAttr(this->agents[i].getX(), this->agents[i].getY(), this->agents[i].getAttr());
			}
		} else {
			this->canmoveAgents[i] = true;
		}
	}

	//とりあえずSTOPにセット
	std::for_each(this->agents.begin(), this->agents.end(), [&, this](auto& a){
			a.move(STOP);
		});
}

UF Field::makePureTreeMine() {
    UF pureTree(field.size());

    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            if(!this->at(j,i)->isPurePanel() && !this->at(j,i)->isEnemyPanel()) continue;
    		    // UP
                if(i-1 >= 0) {
                        if(this->at(j, i-1)->isPurePanel() || this->at(j, i-1)->isEnemyPanel()) {
                                pureTree.unite(xyIndex(j,i), xyIndex(j,i-1));
                        }
                }
                // DOWN
                if(i+1 < height) {
                        if(this->at(j, i+1)->isPurePanel() || this->at(j, i+1)->isEnemyPanel()) {
                                pureTree.unite(xyIndex(j,i), xyIndex(j,i+1));
                        }
                }
                // RIGHT
                if(j+1 < width) {
                        if(this->at(j+1, i)->isPurePanel() || this->at(j+1, i)->isEnemyPanel()) {
                                pureTree.unite(xyIndex(j,i), xyIndex(j+1, i));
                        }
                }
                // LEFT
                if(j-1 >= 0) {
                        if(this->at(j-1, i)->isPurePanel()  || this->at(j-1, i)->isEnemyPanel()) {
                                pureTree.unite(xyIndex(j,i), xyIndex(j-1, i));
                        }
                }
		}
	}
	return pureTree;
}

UF Field::makePureTreeEnemy() {
    UF pureTree(field.size());

    for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
            if(!this->at(j,i)->isPurePanel() && !this->at(j,i)->isMyPanel()) continue;
    		    // UP
                if(i-1 >= 0) {
                        if(this->at(j, i-1)->isPurePanel() || this->at(j, i-1)->isMyPanel()) {
                                pureTree.unite(xyIndex(j,i), xyIndex(j,i-1));
                        }
                }
                // DOWN
                if(i+1 < height) {
                        if(this->at(j, i+1)->isPurePanel() || this->at(j, i+1)->isMyPanel()) {
                                pureTree.unite(xyIndex(j,i), xyIndex(j,i+1));
                        }
                }
                // RIGHT
                if(j+1 < width) {
                        if(this->at(j+1, i)->isPurePanel() || this->at(j+1, i)->isMyPanel()) {
                                pureTree.unite(xyIndex(j,i), xyIndex(j+1, i));
                        }
                }
                // LEFT
                if(j-1 >= 0) {
                        if(this->at(j-1, i)->isPurePanel()  || this->at(j-1, i)->isMyPanel()) {
                                pureTree.unite(xyIndex(j,i), xyIndex(j-1, i));
                        }
                }
		}
	}
	return pureTree;
}

std::unordered_map<int_fast32_t , std::vector<int_fast32_t>> Field::makePureTerritory(UF &&pureTree) {
	std::unordered_map<int_fast32_t, std::vector<int_fast32_t>> retn;
	for(size_t i = 0; i < height; i++) {
		for(size_t j = 0; j < width; j++) {
			if(this->at(j, i)->isPurePanel()) retn[pureTree.root(xyIndex(j,i))].push_back(xyIndex(j, i));
		}
	}
	return retn;
}

int_fast32_t Field::calcMinepanelScore() {
	int_fast32_t tmpScore = 0;
	for(auto &i: field) {
		if(i.isMyPanel()) tmpScore += i.getValue();
	}
	return tmpScore;
}

int_fast32_t Field::calcEnemypanelScore() {
	int_fast32_t tmpScore = 0;
	for(auto &i: field) {
		if(i.isEnemyPanel()) tmpScore += i.getValue();
	}
	return tmpScore;
}

bool Field::isPanelMineBetween(uint_fast32_t x, uint_fast32_t y) {
	int_fast32_t buf = 0;
	
	// left
	for(size_t i = x-1; i >= 0; i--) {
		if(this->at(i, y)->isMyPanel()) {
			buf++;
			break;
		}
	}
	if(buf == 0) return false;

	// right
	for(size_t i = x+1; i < width; i++) {
		if(this->at(i, y)->isMyPanel()) {
			buf++;
			break;
		}
	}
	if(buf == 1) return false;

	// up
	for(size_t i = y-1; i >= 0; i--) {
		if(this->at(x, i)->isMyPanel()) {
			buf++;
			break;
		}
	}
	if(buf == 2) return false;

	// down
	for(size_t i = y+1; i < height; i++) {
		if(this->at(x, i)->isMyPanel()) {
			buf++;
			break;
		}
	}
	if(buf == 3) return false;
	return true;
}

bool Field::isPanelEnemyBetween(uint_fast32_t x, uint_fast32_t y) {
	int_fast32_t buf = 0;
	
	// left
	for(size_t i = x-1; i >= 0; i--) {
		if(this->at(i, y)->isEnemyPanel()) {
			buf++;
			break;
		}
	}
	if(buf == 0) return false;

	// right
	for(size_t i = x+1; i < width; i++) {
		if(this->at(i, y)->isEnemyPanel()) {
			buf++;
			break;
		}
	}
	if(buf == 1) return false;

	// up
	for(size_t i = y-1; i >= 0; i--) {
		if(this->at(x, i)->isEnemyPanel()) {
			buf++;
			break;
		}
	}
	if(buf == 2) return false;

	// down
	for(size_t i = y+1; i < height; i++) {
		if(this->at(x, i)->isEnemyPanel()) {
			buf++;
			break;
		}
	}
	if(buf == 3) return false;
	return true;
}

bool Field::checkLocalArea(uint_fast32_t x, uint_fast32_t y, uint_fast32_t attr) {
	if(attr == MINE_ATTR) return isPanelMineBetween(x, y);
	return isPanelEnemyBetween(x, y);
}

int_fast32_t Field::calcMineScore(std::unordered_map<int_fast32_t, std::vector<int_fast32_t>> &pureTree) {
	int_fast32_t totalscore = 0, score;
	bool check;

	for(const auto &[key, vec]: pureTree) {
		check = true;
		score = 0;
		for(auto pn: vec) {
			if(!checkLocalArea(indexX(pn), indexY(pn), MINE_ATTR)) {
				check = false;
				break;
			}
			score += std::abs(field.at(pn).getValue());
		}
		if(check == true) {
			totalscore += score;
		}
	}
	return totalscore;
}

const Panel *Field::at(uint_fast32_t x, uint_fast32_t y) const {
	return (const Panel *)&(this->field[x + (y << this->yShiftOffset)]);
}

int_fast32_t Field::calcEnemyScore(std::unordered_map<int_fast32_t, std::vector<int_fast32_t>> &pureTree) {
	int_fast32_t totalscore = 0, score;
	bool check;

	for(const auto &[key, vec]: pureTree) {
		check = true;
		score = 0;
		for(auto pn: vec) {
			if(!checkLocalArea(indexX(pn), indexY(pn), ENEMY_ATTR)) {
				check = false;
				break;
			}
			score += std::abs(field.at(pn).getValue());
		}
		if(check == true) {
			totalscore += score;
		}
	}
	return totalscore;
}

int_fast32_t Field::calcScore(uint_fast32_t attr) {
	int_fast32_t score = 0;
	std::unordered_map<int_fast32_t, std::vector<int_fast32_t>> pureTerritory;

	if(attr == MINE_ATTR) {
		score += calcMinepanelScore();
		pureTerritory = makePureTerritory(makePureTreeMine());
		score += calcMineScore(pureTerritory);
	} else {
		score += calcEnemypanelScore();
		pureTerritory = makePureTerritory(makePureTreeEnemy());
		score += calcEnemyScore(pureTerritory);
	}

	return score;
}

void Field::testMoveAgent() {
	Direction buf;
	for(auto &i: this->agents) {
RE_CONSIDER:
		buf = (Direction)this->random(DIRECTION_SIZE - 3);
		if(canMove(i, buf)) i.move(buf);
		else goto RE_CONSIDER; // もし動ける方向でなければ方向を検討し直す
	}	

	applyNextAgents();
}


void Field::print() {
	uint_fast32_t flag;
	char strip[] = "----------------------------------------\n";

	printf("%s", strip);
	for(size_t i = 0; i < this->height; i++) {
		for(size_t j = 0; j < this->width; j++) {
			// パネルの属性の表示処理
			if(this->at(j, i)->isMyPanel()) printf("\x1b[34m");
			if(this->at(j, i)->isEnemyPanel()) printf("\x1b[31m"); 
			
			// エージェントの表示処理
			// ||agents|| が十分に小さいため線形探索でも計算時間にそれほど影響がでない
			flag = PURE_ATTR;
			for(auto &a: this->agents) {
				if(a.getX() == j && a.getY() == i) {
					flag = a.getAttr();
					break;
				}
			}
			if(flag == MINE_ATTR) printf("\x1b[34m");
			if(flag == ENEMY_ATTR) printf("\x1b[31m");
			printf("%3d ", this->at(j, i)->getValue());
			if(flag != PURE_ATTR) printf("\x1b[39m");
			if(!this->at(j, i)->isPurePanel()) printf("\x1b[39m");
		}
		printf("\n");
	}
	printf("%s", strip);
	for(size_t i = 0; i < this->agents.size(); i++) {
		if(this->agents[i].getAttr() == MINE_ATTR) printf("\x1b[34m");
		else printf("\x1b[31m");
		printf("agent[%u]: (%u, %u)\n\x1b[39m", i, this->agents[i].getX(), this->agents[i].getY());
	}
	printf("mineScore:  %d\n", this->calcScore(MINE_ATTR));
	
	printf("enemyScore: %d\n", this->calcScore(ENEMY_ATTR));
	
	printf("%s", strip);
}

const uint_fast32_t Field::getWidth() const{
	return this->width;
}

const uint_fast32_t Field::getHeight() const{
	return this->height;
}
