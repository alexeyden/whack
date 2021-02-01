#ifndef EMPTYBHV_H
#define EMPTYBHV_H

#include "behaviour.h"

class EmptyBhv : public Behaviour
{
public:
	EmptyBhv(Enemy* owner) : Behaviour(owner) {}
	
	virtual void damage(float damage);
	virtual void collision(float x, float y, float z);
	virtual void update(double dt);
};

#endif // EMPTYBHV_H
