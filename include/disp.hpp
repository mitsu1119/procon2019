#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <GL/glut.h>
#include "field.hpp"

class Display;
class Field;

class DisplayWrapper{
private:
	
	static DisplayWrapper* instance;
	static const unsigned int window_width_size=800;
	static const unsigned int window_height_size=800;
	static const unsigned int window_width_position=100;
	static const unsigned int window_height_position=100;
	
public:
	
	Field* field;
	
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
	
	virtual void resize(int w, int h)=0;
	virtual void display()=0;
	virtual void keyboard(unsigned char key, int x, int y)=0;
	virtual void specialKeyboard(int key, int x, int y)=0;
	virtual void mouse(int button, int state, int x, int y)=0;
	virtual void motion(int x, int y)=0;
	
};

class Display : public DisplayWrapper{
private:
	
	static const unsigned int line_size=1;
	static const unsigned int cell_size=30;
	static const unsigned int agent_size=15;
	static const unsigned int panel_size=30;
	
public:
	Display();
	~Display();
	
	void resize(int w, int h) override;
	void display() override;
	void keyboard(unsigned char key, int x, int y) override;
	void specialKeyboard(int key, int x, int y) override;
	void mouse(int button, int state, int x, int y) override;
	void motion(int x, int y) override;

	const void line();
	const void score();
	const void panel();
	const void agent();
	
	const void renderString(float x, float y, const std::string& str);
};

