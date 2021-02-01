#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "util/math.h"

class Enemy;

class Behaviour
{
public:
	Behaviour(Enemy* owner) : owner(owner) {}
	
	virtual void update(double dt) = 0;
	
	virtual void collision(float x, float y, float z) = 0;
	virtual void damage(float damage) = 0;
	
protected:
	Enemy* owner;
};

#endif // BEHAVIOUR_H
