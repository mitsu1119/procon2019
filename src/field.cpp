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
	this->agents.emplace_back(width - 3, 2, MINE_ATTR);
	this->agents.emplace_back(2, height - 3, MINE_ATTR);
	this->agents.emplace_back(width - 2, height - 2, MINE_ATTR);

	this->agents.emplace_back(2, 2, ENEMY_ATTR);
	this->agents.emplace_back(width - 2, 1, ENEMY_ATTR);
	this->agents.emplace_back(1, height - 2, ENEMY_ATTR);
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
  std::cerr << "[*]field_sum:" << field_rand_sum << std::endl;

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
		for(size_t j = 0; j < this->agents.size(); j++) {
			if(i == j || this->agents[i].getnextX() != this->agents[j].getnextX() ||  this->agents[i].getnextY() != this->agents[j].getnextY()) continue;
			this->canmoveAgents[i] = false;
			this->canmoveAgents[j] = false;
			this->agents[i].move(STOP);
			this->agents[j].move(STOP);
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

int_fast32_t Field::isInsideClosed(std::vector<int_fast32_t> &vec) {
	uint_fast32_t x, y;
	int_fast32_t absscore = 0;
	for(auto pn: vec) {
		x = indexX(pn);
		y = indexY(pn);
		if(!isEdge(x, y)) absscore += std::abs(this->at(x, y)->getValue());
		else {
			absscore = -1;
			break;
		}
	}
	return absscore;
}

int_fast32_t Field::calcMineScore(UF &pureTree) {
	int_fast32_t totalscore = 0, root;
	std::unordered_map<int_fast32_t, std::pair<bool, int_fast32_t>> scoreAndClosed;

	for(size_t i = 0; i < height; i++) {
		for(size_t j = 0; j < width; j++) {
			if(this->at(j, i)->isPurePanel()) {
				root = pureTree.root(xyIndex(j, i));
				if(scoreAndClosed[root].first == false) {
					if(isEdge(j, i)) {
						scoreAndClosed[root].first = true;
						totalscore -= scoreAndClosed[root].second;
					} else {
						scoreAndClosed[root].second += std::abs(this->at(j, i)->getValue());
						totalscore += std::abs(this->at(j, i)->getValue());
					}
				}
			}
		}
	}

	return totalscore;
}

const Panel *Field::at(uint_fast32_t x, uint_fast32_t y) const {
	return (const Panel *)&(this->field[x + (y << this->yShiftOffset)]);
}

int_fast32_t Field::calcEnemyScore(UF &pureTree) {
	int_fast32_t totalscore = 0, root;
	std::unordered_map<int_fast32_t, std::pair<bool, int_fast32_t>> scoreAndClosed;

	for(size_t i = 0; i < height; i++) {
		for(size_t j = 0; j < width; j++) {
			if(this->at(j, i)->isPurePanel()) {
				root = pureTree.root(xyIndex(j, i));
				if(scoreAndClosed[root].first == false) {
					if(isEdge(j, i)) {
						scoreAndClosed[root].first = true;
						totalscore -= scoreAndClosed[root].second;
					} else {
						scoreAndClosed[root].second += std::abs(this->at(j, i)->getValue());
						totalscore += std::abs(this->at(j, i)->getValue());
					}
				}
			}
		}
	}

	return totalscore;
}

int_fast32_t Field::calcScore(uint_fast32_t attr) {
	int_fast32_t score = 0;
	std::unordered_map<int_fast32_t, std::vector<int_fast32_t>> pureTerritory;

	if(attr == MINE_ATTR) {
		score += calcMinepanelScore();
		UF pureTree = makePureTreeMine();
		score += calcMineScore(pureTree);
	} else {
		score += calcEnemypanelScore();
		UF pureTree = makePureTreeEnemy();
		score += calcEnemyScore(pureTree);
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

	fprintf(stderr, "%s", strip);
	for(size_t i = 0; i < this->height; i++) {
		for(size_t j = 0; j < this->width; j++) {
			// パネルの属性の表示処理
			if(this->at(j, i)->isMyPanel()) fprintf(stderr, "\x1b[34m");
			if(this->at(j, i)->isEnemyPanel()) fprintf(stderr, "\x1b[31m"); 
			// エージェントの表示処理
			// ||agents|| が十分に小さいため線形探索でも計算時間にそれほど影響がでない
			flag = PURE_ATTR;
			for(auto &a: this->agents) {
				if(a.getX() == j && a.getY() == i) {
					flag = a.getAttr();
					break;
				}
			}
			if(flag == MINE_ATTR) fprintf(stderr, "\x1b[34m");
			if(flag == ENEMY_ATTR) fprintf(stderr, "\x1b[31m");
			fprintf(stderr, "%3d ", this->at(j, i)->getValue());
			if(flag != PURE_ATTR) fprintf(stderr, "\x1b[39m");
			if(!this->at(j, i)->isPurePanel()) fprintf(stderr, "\x1b[39m");
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "%s", strip);
	for(size_t i = 0; i < this->agents.size(); i++) {
		if(this->agents[i].getAttr() == MINE_ATTR) fprintf(stderr, "\x1b[34m");
		else fprintf(stderr, "\x1b[31m");
		fprintf(stderr, "agent[%u]: (%u, %u)\n\x1b[39m", i, this->agents[i].getX(), this->agents[i].getY());
	}
	fprintf(stderr, "mineScore:  %d\n", this->calcScore(MINE_ATTR));
	fprintf(stderr, "enemyScore: %d\n", this->calcScore(ENEMY_ATTR));
	fprintf(stderr, "turn:       %d\n", this->turn);
	fprintf(stderr, "%s", strip);

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
	fprintf(stderr, "\n---------------- finish ----------------\n");
	int_fast32_t mineScore = this->calcScore(MINE_ATTR);
	int_fast32_t enemyScore = this->calcScore(ENEMY_ATTR);

	printf("%d\n", mineScore - enemyScore);
	if(mineScore > enemyScore){
		printf("Win  MINE\n");
		printf("Lose ENEMY\n");
	} else if(mineScore == enemyScore){
		printf("Draw\n");
	} else if(mineScore < enemyScore){
		printf("Lose MINE\n");
		printf("Win  ENEMY\n");
	}
}

void Field::init(){
	// agetn情報
	int agent_data[30][3];
  
  int index_num = 0;
	int agentid, x, y;
	int agent_num = 0;

  int my_attr_tmp; // TeamID用のjson添字
  int en_attr_tmp;

	std::vector<int> map;
	std::vector<int> tiles;
	double buffer;
	value maps;
	{
		std::fstream stream("./../../public_field/fields.json");
		if(!stream.is_open()) return 1;
		stream >> maps;
		assert(get_last_error().empty());
		stream.close();
	}

  value matches;
  {
    std::fstream stream("./../../matches.json");
    if(!stream.is_open()) return 1;
    stream >> matches;
    assert(get_last_error().empty());
    stream.close();
  }

	this->height = (int)maps.get<object>()["height"].get<double>();
	this->width  = (int)maps.get<object>()["width"].get<double>();

	double buff;
	uint_fast32_t sizee;

	buff = std::log2(this->width);
	this->yShiftOffset = (uint_fast32_t)(buff + ((std::ceil(buff) == std::floor(buff)) ? 0 : 1));
	sizee = this->height << this->yShiftOffset;

	// マップ生成
	this->field = std::vector<Panel>(sizee, Panel(0));
	
	value::array points = maps.get<object>()["points"].get<value::array>();
	for(int i = 0; i < height; i++){
		value::array pt = points[i].get<value::array>();
		for(value item : pt){
			buffer = item.get<double>();
			map.push_back((int)buffer);
		}
	}

	value::array tile = maps.get<object>()["tiled"].get<value::array>();
	for(int i = 0; i < height; i++){
		value::array tl = tile[i].get<value::array>();
		for(value item : tl){
			buffer = item.get<double>();
			tiles.push_back((int)buffer);
		}
	}
  

	value::array agents = maps.get<object>()["teams"].get<value::array>(); // all agent

	// teamID  <- 別のスクリプトで取ってくる必要あり
	int myID = (int)agents[0].get<object>()["teamID"].get<double>();
	int enID = (int)agents[1].get<object>()["teamID"].get<double>();
  // これ
  int myTeamID = (int)matches.get<object>()["teamID"].get<double>();
	int enTeamID;
	if (myTeamID == myID){
		enTeamID = enID;
	}else{
		enTeamID = myID;
	}

  if (myTeamID == myID){
    my_attr_tmp = 0;
    en_attr_tmp = 1;
  }else{
    my_attr_tmp = 1;
    en_attr_tmp = 0;
  }

	// agent array
	value::array myagents = agents[my_attr_tmp].get<object>()["agents"].get<value::array>(); // my
	value::array enagents = agents[en_attr_tmp].get<object>()["agents"].get<value::array>(); // enemy

	// Debug---------------
	std::cerr << "\n\n";
  std::cerr << "[*] turn    :" << turn << std::endl; // Debug
	// std::cerr << "[*] turns   :" << end_turn   << std::endl; // print turns
	std::cerr << "[*] height  :" << height << std::endl; // print height
	std::cerr << "[*] width   :" << width  << std::endl; // print width
	std::cerr << "[*] myTeamID:"  << myID  << std::endl; // print myTeamID
	std::cerr << "[*] enTeamID:"  << enID  << std::endl; // print enemyTeamID
	std::cerr << std::endl;

	// print myagent array
	std::cerr << "[*] myagent_array:" << std::endl;
	for(value item : myagents){
		agentid = (int)item.get<object>()["agentID"].get<double>();
		x       = (int)item.get<object>()["x"].get<double>();
		y       = (int)item.get<object>()["y"].get<double>();
		std::cerr << "agentID:" << (int)item.get<object>()["agentID"].get<double>();
		std::cerr << "  x:" << (int)item.get<object>()["x"].get<double>();
		std::cerr << "  y:" << (int)item.get<object>()["y"].get<double>() << std::endl;
		agent_data[index_num][0] = x;
		agent_data[index_num][1] = y;
    // index (Debug) ------------------------
    agent_data[index_num][2] = agentid;
    index_num += 1;
		// agent数を求める
		agent_num += 1;
	}
	std::cerr << std::endl;

	// print enagent array
	std::cerr << "[*] enagent_array:" << std::endl;
	for(value item : enagents){
		agentid = (int)item.get<object>()["agentID"].get<double>();
		x       = (int)item.get<object>()["x"].get<double>();
		y       = (int)item.get<object>()["y"].get<double>();
		std::cerr << "agentID:" << (int)item.get<object>()["agentID"].get<double>();
		std::cerr << "  x:" << (int)item.get<object>()["x"].get<double>();
		std::cerr << "  y:" << (int)item.get<object>()["y"].get<double>() << std::endl;
		agent_data[index_num][0] = x;
		agent_data[index_num][1] = y;

    // index (Debug) ---------------------------
    agent_data[index_num][2] = agentid;
    index_num += 1;
	}


	//genRandMap(仮)
	std::vector<int> field_rand;
  std::vector<int> buf;
	int map_num = 0; //map用添字
	int buf_height, buf_width;
  int field_rand_sum = -1; // <- Debug

	buf_height=(this->height/2)+(this->height%2);
	buf_width=(this->width/2)+(this->width%2);

  while(field_rand_sum < 0){
    field_rand_sum = 0; // <- Debug
    for(int i=0; i < height; i++){
      for(int j=0; j < width; j++){
        buf.push_back((int)map[map_num]);

				map_num += 1;
      }
      field_rand.insert(field_rand.end(), buf.begin(), buf.end());
      buf.clear();
    }
    field_rand_sum = std::accumulate(field_rand.begin(),field_rand.end(),0);  // <- Debug
  }

  // showing field_sum pt ---------------------------------
  std::cerr << "[*]field_sum:" << field_rand_sum << std::endl;

	int val=0;

	for(int i=0;i<this->height;i++){
		for(int j=0;j<this->width;j++){
			this->setPanelScore(j, i, field_rand.at(val));
			val++;
		}
	}

	val = 0;
	for(int i=0;i<this->height;i++){
		for(int j=0;j<this->width;j++){
			if(tiles[val] == myTeamID)
				this->setPanelAttr(j, i, MINE_ATTR);
			else if(tiles[val] == enTeamID)
				this->setPanelAttr(j, i, ENEMY_ATTR);
			else
				this->setPanelAttr(j, i, PURE_ATTR);
			val++;
		}
	}

	std::cout << "agents:" << agent_num << std::endl;
	// Debug -----------------------------------------
  // TODO: 汎用的な実装にする(agentIDとATTR)
  // このままだと自チームが敵側の場合OUT
	agent_num *= 2;
	for(int s=0; s < agent_num; s++){
		if(s < agent_num/2){
			this->agents.emplace_back(agent_data[s][0]-1, agent_data[s][1]-1, MINE_ATTR, agent_data[s][2]);
		}
		else{
			this->agents.emplace_back(agent_data[s][0]-1, agent_data[s][1]-1, ENEMY_ATTR, agent_data[s][2]);
		}
	}

	//ターン
	this->turn = 0;

	// 終了ターン
	this->max_turn = (int)matches.get<object>()["turns"].get<double>();
	//this->max_turn = 2;
	std::cerr << "max_turn:" << this->max_turn << std::endl;
	
	//シード値設定
	this->random = XorOshiro128p(time(NULL));
	
	this->canmoveAgents = std::vector<bool>(this->agents.size(), true);

	//とりあえずSTOPにセット
	std::for_each(this->agents.begin(), this->agents.end(), [&, this](auto& a){
			a.move(STOP);
		});
}

bool Field::is_inside_closed(const std::pair<uint_fast32_t, uint_fast32_t>& coord) const{
	int_fast32_t totalscore = 0, score, x, y;
	bool check, flag;
	
	std::unordered_map<int_fast32_t, std::vector<int_fast32_t>> pureTerritory;
	pureTerritory = makePureTerritory(makePureTreeMine());
	
	for(const auto &[key, vec]: pureTerritory) {
		check = true;
		score = 0;
		for(auto pn: vec) {
			if(!checkLocalArea(indexX(pn), indexY(pn), MINE_ATTR)) {
				check = false;
				break;
			}
			x = this->indexX(pn);
			y = this->indexY(pn);
			if(x == coord.first && y == coord.second)
				flag = true;
		}
		if(check == true || flag == true)
			return true;
	}
	return false;
}

void Field::setPanels(const std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>>& decided_route, const uint_fast32_t attr, const uint_fast32_t depth) const{
	std::for_each(decided_route.begin(), decided_route.end(),[&, this](auto& route){
			for(size_t i = 0; i < depth; i++){
				if(i >= route.size())
					break;
				this->setPanelAttr(route.at(i).first, route.at(i).second, attr);
			}
		});
}

void Field::update(){
	// agetn情報
	int agent_data[30][3];
  
  int index_num = 0;
	int agentid, x, y;
	int agent_num = 0;

  int my_attr_tmp; // TeamID用のjson添字
  int en_attr_tmp;

	std::vector<int> map;
	std::vector<int> tiles;
	double buffer;
	value maps;
	{
		std::fstream stream("./../../public_field/fields.json");
		if(!stream.is_open()) return 1;
		stream >> maps;
		assert(get_last_error().empty());
		stream.close();
	}

  value matches;
  {
    std::fstream stream("./../../matches.json");
    if(!stream.is_open()) return 1;
    stream >> matches;
    assert(get_last_error().empty());
    stream.close();
  }

	double buff;
	uint_fast32_t sizee;

	buff = std::log2(this->width);
	this->yShiftOffset = (uint_fast32_t)(buff + ((std::ceil(buff) == std::floor(buff)) ? 0 : 1));
	sizee = this->height << this->yShiftOffset;

	// マップ生成
	value::array tile = maps.get<object>()["tiled"].get<value::array>();
	for(int i = 0; i < height; i++){
		value::array tl = tile[i].get<value::array>();
		for(value item : tl){
			buffer = item.get<double>();
			tiles.push_back((int)buffer);
		}
	}

	value::array agents = maps.get<object>()["teams"].get<value::array>(); // all agent

	// teamID  <- 別のスクリプトで取ってくる必要あり
	int myID = (int)agents[0].get<object>()["teamID"].get<double>();
	int enID = (int)agents[1].get<object>()["teamID"].get<double>();
  // これ
  int myTeamID = (int)matches.get<object>()["teamID"].get<double>();
	int enTeamID;
	if (myTeamID == myID){
		enTeamID = enID;
	}else{
		enTeamID = myID;
	}

  if (myTeamID == myID){
    my_attr_tmp = 0;
    en_attr_tmp = 1;
  }else{
    my_attr_tmp = 1;
    en_attr_tmp = 0;
  }

	// agent array
	value::array myagents = agents[my_attr_tmp].get<object>()["agents"].get<value::array>(); // my
	value::array enagents = agents[en_attr_tmp].get<object>()["agents"].get<value::array>(); // enemy

	// Debug---------------
	std::cerr << "\n\n";
  std::cerr << "[*] turn    :" << turn << std::endl; // Debug
	// std::cerr << "[*] turns   :" << end_turn   << std::endl; // print turns
	std::cerr << "[*] height  :" << height << std::endl; // print height
	std::cerr << "[*] width   :" << width  << std::endl; // print width
	std::cerr << "[*] myTeamID:"  << myID  << std::endl; // print myTeamID
	std::cerr << "[*] enTeamID:"  << enID  << std::endl; // print enemyTeamID
	std::cerr << std::endl;

	// print myagent array
	std::cerr << "[*] myagent_array:" << std::endl;
	for(value item : myagents){
		agentid = (int)item.get<object>()["agentID"].get<double>();
		x       = (int)item.get<object>()["x"].get<double>();
		y       = (int)item.get<object>()["y"].get<double>();
		std::cerr << "agentID:" << (int)item.get<object>()["agentID"].get<double>();
		std::cerr << "  x:" << (int)item.get<object>()["x"].get<double>();
		std::cerr << "  y:" << (int)item.get<object>()["y"].get<double>() << std::endl;
		agent_data[index_num][0] = x;
		agent_data[index_num][1] = y;
    // index (Debug) ------------------------
    agent_data[index_num][2] = agentid;
    index_num += 1;
		// agent数を求める
		agent_num += 1;
	}
	std::cerr << std::endl;

	// print enagent array
	std::cerr << "[*] enagent_array:" << std::endl;
	for(value item : enagents){
		agentid = (int)item.get<object>()["agentID"].get<double>();
		x       = (int)item.get<object>()["x"].get<double>();
		y       = (int)item.get<object>()["y"].get<double>();
		std::cerr << "agentID:" << (int)item.get<object>()["agentID"].get<double>();
		std::cerr << "  x:" << (int)item.get<object>()["x"].get<double>();
		std::cerr << "  y:" << (int)item.get<object>()["y"].get<double>() << std::endl;
		agent_data[index_num][0] = x;
		agent_data[index_num][1] = y;

    // index (Debug) ---------------------------
    agent_data[index_num][2] = agentid;
    index_num += 1;
	}

	int val = 0;
	for(int i=0;i<this->height;i++){
		for(int j=0;j<this->width;j++){
			if(tiles[val] == myTeamID)
				this->setPanelAttr(j, i, MINE_ATTR);
			else if(tiles[val] == enTeamID)
				this->setPanelAttr(j, i, ENEMY_ATTR);
			else
				this->setPanelAttr(j, i, PURE_ATTR);
			val++;
		}
	}

	std::cout << "agents:" << agent_num << std::endl;
	// Debug -----------------------------------------
  // TODO: 汎用的な実装にする(agentIDとATTR)
  // このままだと自チームが敵側の場合OUT
	agent_num *= 2;
	std::vector<Agent>().swap(this->agents);
	for(int s=0; s < agent_num; s++){
		if(s < agent_num/2){
			this->agents.emplace_back(agent_data[s][0]-1, agent_data[s][1]-1, MINE_ATTR, agent_data[s][2]);
		}
		else{
			this->agents.emplace_back(agent_data[s][0]-1, agent_data[s][1]-1, ENEMY_ATTR, agent_data[s][2]);
		}
	}
}
