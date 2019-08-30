#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <utility>
#include <GL/glut.h>
#include "field.hpp"
#include "ai.hpp"

class Field;

const std::vector<int> vec_x = {0, 1, 1, 1, 0, -1, -1, -1, 0};
const std::vector<int> vec_y = {-1, -1, 0, 1, 1, 1, 0, -1, 0};

constexpr uint_fast32_t cell_size  = 30;
constexpr uint_fast32_t half = cell_size / 2;

constexpr uint_fast32_t line_size  = 1;
constexpr uint_fast32_t panel_size = 30;
constexpr uint_fast32_t agent_size = 15;
constexpr uint_fast32_t point_size = 15;

class Print{
private:

private:

	void renderString(float x, float y, const std::string& str) const;
	
public:

	Print();
	~Print();

	void line(const Field* field) const;
	void score(const Field* field) const;
	void panel(const Field* field) const;
	void agent(Field* field) const;
	void agentNum(Field* field) const;
	void point(const Field* field) const;
	void candidate(Field* field, const std::vector<Direction>& next) const;

	void line(const Field& field) const;
	void score(const Field& field) const;
	void panel(const Field& field) const;
	void agent(Field& field) const;
	void agentNum(Field& field) const;
	void point(const Field& field) const;
	void candidate(Field& field, const std::vector<Direction>& next) const;

	virtual void print(Field* field, const std::vector<Direction> next) = 0;
	virtual void print(Field* field) = 0;
	virtual void print(Field& field) = 0;
	
};

class PrintDisplay : public Print{
private:
public:
	
	PrintDisplay();
	~PrintDisplay();
	
	void print(Field* field, const std::vector<Direction> next) override;
	void print(Field* field) override;
	void print(Field& field) override;
	
};

class PrintSelfDirectedGame : public Print{
private:
public:
	
	PrintSelfDirectedGame();
	~PrintSelfDirectedGame();
	
	void print(Field* field, const std::vector<Direction> next) override;
	void print(Field* field) override;
	void print(Field& field) override;
	
};

class Display;
class AI;
class RandomMine;
class RandomEnemy;

constexpr unsigned int window_width_size      = 800;
constexpr unsigned int window_height_size     = 800;
constexpr unsigned int window_width_position  = 100;
constexpr unsigned int window_height_position = 100;

class DisplayWrapper{
private:

	static DisplayWrapper* instance;

public:

	Field* field;
	Print* print;
	
	AI* random;
	AI* astar;
	AI* greedy;

	
	AI* breadth_force_search;

	

	DisplayWrapper();
	virtual ~DisplayWrapper();
	
	void init();
	void start(int argc, char *argv[]);
	void setField(Field* object);
	
	static void resizeWrapper(int w, int h);
	static void displayWrapper();
	static void keyboardWrapper(unsigned char key, int x, int y);
	static void specialKeyboardWrapper(int key, int x, int y);
	static void mouseWrapper(int button, int state, int x, int y);
	static void motionWrapper(int x, int y);
	static void setInstance(DisplayWrapper* framework);

	virtual void initInstance() = 0;
	virtual void resize(int w, int h) = 0;
	virtual void display() = 0;
	virtual void keyboard(unsigned char key, int x, int y) = 0;
	virtual void specialKeyboard(int key, int x, int y) = 0;
	virtual void mouse(int button, int state, int x, int y) = 0;
	virtual void motion(int x, int y) = 0;
	
};

class Display : public DisplayWrapper{
private:
	
	std::vector<std::vector<std::pair<uint_fast32_t, uint_fast32_t>>> possible;
	std::vector<Direction> next;

	//agent_flag == 0:mineの入力 agent_flag == 1:enemyの入力
	//bool agent_flag;
	//uint_fast32_t enemy_flag;
	
	uint_fast32_t mine_flag;

	/*
	void setPossible();
	void moveNext();
	void selectAgent(uint_fast32_t x, uint_fast32_t y);
	void selectDirection(uint_fast32_t x, uint_fast32_t y);
	*/

	void setPossible();
	void moveNext();
	bool selectAgent(uint_fast32_t x, uint_fast32_t y);
	void selectDirection(uint_fast32_t x, uint_fast32_t y);
	const Direction changeDirection(const std::pair<uint_fast32_t, uint_fast32_t>& now, const std::pair<uint_fast32_t, uint_fast32_t>& next) const;
	
	void init();
	const bool isOutOfRange(int button, int state, int x, int y) const;
	
public:

 	Display();
 	~Display();

	void initInstance();
 	void resize(int w, int h) override;
 	void display() override;
 	void keyboard(unsigned char key, int x, int y) override;
 	void specialKeyboard(int key, int x, int y) override;
 	void mouse(int button, int state, int x, int y) override;
 	void motion(int x, int y) override;
	
};

inline const Direction Display::changeDirection(const std::pair<uint_fast32_t, uint_fast32_t>& now, const std::pair<uint_fast32_t, uint_fast32_t>& next) const{
	for(size_t i = 0; i < DIRECTION_SIZE - 2; i++)
		if(next.first == now.first + vec_x.at(i) && next.second == now.second + vec_y.at(i))
			return (Direction)i;
}

/*
class selfDirectedGame : public DisplayWrapper {
private:
	
public:
	
	selfDirectedGame();
	~selfDirectedGame();
	
	void resize(int w, int h) override;
	void display() override;
	void keyboard(unsigned char key, int x, int y) override;
	void specialKeyboard(int key, int x, int y) override;
	void mouse(int button, int state, int x, int y) override;
	void motion(int x, int y) override;
	
};
*/
