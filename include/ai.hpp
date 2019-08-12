#pragma once

#include "field.hpp"
#include <random>

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

class RandomMine : public AI{
private:
public:
	RandomMine();
	~RandomMine();
	void move(Field *field) override;
};

class RandomEnemy : public AI{
private:
public:
	RandomEnemy();
	~RandomEnemy();
	void move(Field *field) override;
};
