#pragma once

#include "field.hpp"

class Field;

class AI{
private:
public:
	AI();
	~AI();
	virtual void move(Field *field)=0;
};

class Astar : public AI{
private:
public:
	Astar();
	~Astar();
	void move(Field *field) override;
};

class Random : public AI{
private:
	XorOshiro128p random;
public:
	Random();
	~Random();
	void move(Field *field) override;
};
