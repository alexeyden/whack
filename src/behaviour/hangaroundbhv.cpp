#include "hangaroundbhv.h"

#include <cstdlib>
#include <cmath>

#include "objects/enemy.h"

HangAroundBhv::HangAroundBhv(Enemy* owner):
	Behaviour(owner), _time(0), _walking(0)
{}

void HangAroundBhv::collision(float x, float y, float z)
{
    (void) x; (void) y; (void) z;
    
	_walking = false;
	_time = 0;
	owner->speedXY(0,0);
	
	// bumped into something, turn around
	owner->dir(owner->dir() - M_PI + float(rand() % 180 - 90) * M_PI/360.0f);
}

void HangAroundBhv::update(double dt)
{
	_time += dt;
	
	if(_walking && _time > maxWalkTime) {
		_time = 0;
		_walking = false;
		owner->speedXY(0,0);
		
		//walk in random direction
		owner->dir(float(rand() % 360) * M_PI/360.0f);
	}
	else if(!_walking && _time > maxStandTime) {
		_time = 0;
		_walking = true;
		
		owner->speedXY(
			owner->minSpeed() *  cos(owner->dir()),
			owner->minSpeed() *  sin(owner->dir())
		);
	}
}
