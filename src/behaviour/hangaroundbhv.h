#ifndef HANGAROUNDBHV_H
#define HANGAROUNDBHV_H

#include "behaviour.h"

class HangAroundBhv : public Behaviour
{
public:
	HangAroundBhv(Enemy* owner);
	
	virtual void update(double dt);
	virtual void collision(float x, float y, float z);
	virtual void damage(float damage) { (void) damage; }

private:
	float _time;
	bool _walking;
	
	const float maxStandTime = 1.0f;
	const float maxWalkTime = 5.0f;
};

#endif // HANGAROUNDBHV_H
