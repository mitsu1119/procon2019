#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <utility>
#include <GL/glut.h>
#include "field.hpp"
#include "ai.hpp"

class Display;
class Field;
class AI;

class DisplayWrapper{
private:

	AI* mine;
	AI* enemy;

	static DisplayWrapper* instance;
	static const unsigned int window_width_size=800;
	static const unsigned int window_height_size=800;
	static const unsigned int window_width_position=100;
	static const unsigned int window_height_position=100;

	static const unsigned int line_size=1;
	static const unsigned int agent_size=15;
	static const unsigned int panel_size=30;
	
public:

	static const unsigned int cell_size=30;
	
	Field* field;

	DisplayWrapper();
	virtual ~DisplayWrapper();
	
	void init();
	void start(int argc, char *argv[]);
	void setField(Field* object);
	void reverseBoard(Field& field);
	
	static void resizeWrapper(int w, int h);
	static void displayWrapper();
	static void keyboardWrapper(unsigned char key, int x, int y);
	static void specialKeyboardWrapper(int key, int x, int y);
	static void mouseWrapper(int button, int state, int x, int y);
	static void motionWrapper(int x, int y);
	static void setInstance(DisplayWrapper* framework);

	virtual void initInstance()=0;
	virtual void resize(int w, int h)=0;
	virtual void display()=0;
	virtual void keyboard(unsigned char key, int x, int y)=0;
	virtual void specialKeyboard(int key, int x, int y)=0;
	virtual void mouse(int button, int state, int x, int y)=0;
	virtual void motion(int x, int y)=0;

	void line() const;
	void score() const;
	void panel() const;
	void agent() const;
 	void renderString(float x, float y, const std::string& str) const;

};

class Display : public DisplayWrapper{
private:

	//DECLARE_ENUM(Direction, direction2name, UP, RUP, RIGHT, RDOWN, DOWN, LDOWN, LEFT, LUP, DIRECTION_SIZE);
	//buf = (Direction)this->random(DIRECTION_SIZE - 1);
	std::vector<std::vector<std::pair<int, int>>> possible_list;
	std::vector<Direction> next_list;
	std::vector<int> mine_id;
	std::vector<int> enemy_id;	

	//flag==0:mineの入力 flag==1:enemyの入力
	unsigned int flag;
	unsigned int mine_flag;
	unsigned int enemy_flag;

	void makePossibleList();
	void moveNextList();
	
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
