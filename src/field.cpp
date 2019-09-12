#include "field.hpp"
#include <fstream>
#include <cassert>

using namespace picojson;

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
	this->attr = MINE_ATTR;
}

void Panel::setEnemy() {
	this->attr = ENEMY_ATTR;
}

void Panel::setPure() {
	this->attr = PURE_ATTR;
}

int_fast32_t Panel::getValue() const {
	return this->value;
}

// ---------------------------------------- Field ---------------------------------------------
Field::Field(uint_fast32_t width, uint_fast32_t height):width(width),height(height),turn(0) {
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

	this->max_turn = 70;

}

Field::Field() {
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

void Field::genRandMap(){
	static const int m = 30;
  static const double s = 7.0;
  static const double slide = 25;
  static const double pi = 3.1415926535;

  double x,y;
  double r1,r2;

	int N = 32;
	std::vector<int> field_rand;
  std::vector<int> field_rev;
  std::vector<int> buf;
	int buf_height, buf_width;
  int field_rand_sum = -1; // <- Debug



	buf_height=(this->height/2)+(this->height%2);
	buf_width=(this->width/2)+(this->width%2);

  // fix bug(for even) -----------------------
  // Add while loop
  // ----- INFO ------------------------------
  // (field_rand_sum < m)||(field_rand_sum >= n)
  //
  // field_rand_sum == m + 4
  //
  // if n == m+4
  // field_rand_sum == m
  // -----------------------------------------
  while(field_rand_sum < 0){
    field_rand.clear(); // <- Debug
    field_rev.clear();  // <- Debug
    field_rand_sum = 0; // <- Debug
    for(int i=0; i < buf_height; i++){
      for(int j=0; j < buf_width; j++){
        r2 = this->random(INT_MAX) / 2147483647.1;
        r1 =this->random(INT_MAX)  / 2147483647.1;
        x = s * sqrt(-2 * log(r1)) * cos(2 * pi * r2)+m-slide;
				if((int)x > 16)
					x = 16;
				if((int)x < -16)
					x = -16;
        buf.push_back((int)x);
        //buf.push_back((rand() % N)-16);
      }

      field_rand.insert(field_rand.end(), buf.begin(), buf.end());
      std::reverse(buf.begin(), buf.end());
      field_rand.insert(field_rand.end(), buf.begin(), buf.end());
      buf.clear();
    }

    field_rev = field_rand;
    std::reverse(field_rev.begin(), field_rev.end());
    field_rand.insert(field_rand.begin(), field_rev.begin(), field_rev.end());

    field_rand_sum = std::accumulate(field_rand.begin(),field_rand.end(),0);  // <- Debug
  }
  // showing field_sum pt ---------------------------------
  std::cout << "[*]field_sum:" << field_rand_sum << std::endl;

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
			a.setnextbufXY(a.getnextX(), a.getnextY());
			a.move(STOP);
		});

	//ターンを刻む
	this->turn++;
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

	// For over flow
	if(x == 0 || y == 0 || x == width - 1 || y == height - 1) return false;

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

	// For over flow
	if(x == 0 || y == 0 || x == width - 1 || y == height - 1) return false;

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
	bool check, test;

	for(const auto &[key, vec]: pureTree) {
		check = true;
		score = 0;
		for(auto pn: vec) {
			test = checkLocalArea(indexX(pn), indexY(pn), MINE_ATTR);
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

	printf("turn      : %d\n", this->turn + 1);

	printf("%s", strip);
}

const uint_fast32_t Field::getWidth() const{
	return this->width;
}

const uint_fast32_t Field::getHeight() const{
	return this->height;
}

const uint_fast32_t Field::getTurn() const{
	return this->turn;
}

const uint_fast32_t Field::getMaxTurn() const{
	return this->max_turn;
}

const bool Field::checkEnd() const{
	return (this->turn == (uint_fast32_t)this->max_turn);
}

void Field::judgeWinner(){
	printf("\n---------------- finish ----------------\n");
	if(this->calcScore(MINE_ATTR) > this->calcScore(ENEMY_ATTR)){
		printf("win  MINE\n");
		printf("lose ENEMY\n");
	}
	if(this->calcScore(MINE_ATTR) == this->calcScore(ENEMY_ATTR))
		printf("DRAW\n");
	if(this->calcScore(MINE_ATTR) < this->calcScore(ENEMY_ATTR)){
		printf("win  ENEMY\n");
		printf("lose MINE\n");
	}
}

void Field::init(){
	//ターン
	this->turn = 0;
	
	//シード値設定
	this->random = XorOshiro128p(time(NULL));
	
	// agetn情報
	int_fast32_t agent_data[17][2];
	int_fast32_t agentid, x, y;
	int_fast32_t agent_num = 0;

	std::vector<int_fast32_t> map;
	std::vector<int_fast32_t> tile;
	double_t buffer;
	value maps;
	{
		std::fstream stream("./../../public_field/E-1.json");
		if(!stream.is_open()) return 1;
		stream >> maps;
		assert(get_last_error().empty());
		stream.close();
	}

	//縦横の大きさ
	this->height = (int_fast32_t)maps.get<object>()["height"].get<double_t>();
	this->width  = (int_fast32_t)maps.get<object>()["width"].get<double_t>();


	uint_fast32_t field_size;
  buffer = std::log2(this->width);
	this->yShiftOffset = (uint_fast32_t)(buffer + ((std::ceil(buffer) == std::floor(buffer)) ? 0 : 1));
	field_size = this->height << this->yShiftOffset;

	// マップ生成
	this->field = std::vector<Panel>(field_size, Panel(0));


	// 現在ターン
	//this->turn = (int)maps.get<object>()["turn"].get<double>();


	value::array points = maps.get<object>()["points"].get<value::array>();
	for(int_fast32_t i = 0; i < height; i++){
		value::array pt = points[i].get<value::array>();
		for(value item : pt){
			buffer = item.get<double_t>();
			map.push_back((int_fast32_t)buffer);
		}
	}

	// agent array
	value::array agents   = maps.get<object>()["teams"].get<value::array>(); // all agent
	value::array myagents = agents[0].get<object>()["agents"].get<value::array>(); // my
	value::array enagents = agents[1].get<object>()["agents"].get<value::array>(); // enemy


	// teamID
	int_fast32_t myID = (int)agents[0].get<object>()["teamID"].get<double_t>();
	int_fast32_t enID = (int)agents[1].get<object>()["teamID"].get<double_t>();


	// Debug---------------
	std::cout << "\n\n";
	std::cout << "[*] turn  :"   << turn   << std::endl; // print turn
	std::cout << "[*] height:"   << height << std::endl; // print height
	std::cout << "[*] width :"   << width  << std::endl; // print width
	std::cout << "[*] myTeamID:" << myID   << std::endl; // print myTeamID
	std::cout << "[*] enTeamID:" << enID   << std::endl; // print enemyTeamID
	std::cout << std::endl;

	// print myagent array
	std::cout << "[*] myagent_array:" << std::endl;
	for(value item : myagents){
		agentid = (int_fast32_t)item.get<object>()["agentID"].get<double_t>();
		x       = (int_fast32_t)item.get<object>()["x"].get<double_t>();
		y       = (int_fast32_t)item.get<object>()["y"].get<double_t>();
		std::cout << "agentID:" << (int)item.get<object>()["agentID"].get<double_t>();
		std::cout << "  x:" << (int)item.get<object>()["x"].get<double_t>();
		std::cout << "  y:" << (int)item.get<object>()["y"].get<double_t>() << std::endl;
		agent_data[agentid][0] = x;
		agent_data[agentid][1] = y;
		// agent数を求める
		agent_num += 1;
	}
	std::cout << std::endl;

	// print enagent array
	std::cout << "[*] enagent_array:" << std::endl;
	for(value item : enagents){
		agentid = (int_fast32_t)item.get<object>()["agentID"].get<double>();
		x       = (int_fast32_t)item.get<object>()["x"].get<double>();
		y       = (int_fast32_t)item.get<object>()["y"].get<double>();
		std::cout << "agentID:" << (int_fast32_t)item.get<object>()["agentID"].get<double_t>();
		std::cout << "  x:"     << (int_fast32_t)item.get<object>()["x"].get<double_t>();
		std::cout << "  y:"     << (int_fast32_t)item.get<object>()["y"].get<double_t>() << std::endl;
		agent_data[agentid][0] = x;
		agent_data[agentid][1] = y;
	}


	//genRandMap(仮)
	std::vector<int_fast32_t> field_rand;
  std::vector<int_fast32_t> buf;
	int_fast32_t map_num = 0; //map用添字
	int_fast32_t buf_height, buf_width;
  int_fast32_t field_rand_sum = -1; // <- Debug

	buf_height = (this->height/2)+(this->height%2);
	buf_width  = (this->width/2)+(this->width%2);

  while(field_rand_sum < 0){
    field_rand_sum = 0; // <- Debug
    for(size_t i=0; i < height; i++){
      for(size_t j=0; j < width; j++){
        buf.push_back((int_fast32_t)map[map_num]);

				map_num += 1;
      }
      field_rand.insert(field_rand.end(), buf.begin(), buf.end());
      buf.clear();
    }
    field_rand_sum = std::accumulate(field_rand.begin(),field_rand.end(),0);  // <- Debug
  }

  // showing field_sum pt ---------------------------------
  std::cout << "[*]field_sum:" << field_rand_sum << std::endl;

	int_fast32_t val = 0;
	for(size_t i = 0; i < this->height; i++){
		for(size_t j = 0; j < this->width; j++){
			this->setPanelScore(j, i, field_rand.at(val));
			val++;
		}
	}

	std::cout << "agents:" << agent_num << std::endl;
	agent_num *= 2;
	for(size_t i = 1; i <= agent_num; i++){
		if(i <= agent_num / 2){
			this->agents.emplace_back(agent_data[i][0]-1, agent_data[i][1]-1, MINE_ATTR,  i);
		}
		else{
			this->agents.emplace_back(agent_data[i][0]-1, agent_data[i][1]-1, ENEMY_ATTR, i);
		}
	}

	this->canmoveAgents = std::vector<bool>(this->agents.size(), true);
	
	// エージェントの初期位置のパネルの属性を設定
	for(auto &i: this->agents) {
		setPanelAttr(i.getX(), i.getY(), i.getAttr());
	}

	//とりあえずSTOPにセット
	std::for_each(this->agents.begin(), this->agents.end(), [&, this](auto& a){
			a.move(STOP);
		});

	this->max_turn = 70;
}
