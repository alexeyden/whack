#include "splash.h"

void Splash::update(float dt)
{
	Decoration::update(dt);
	
	_life += dt;
	
	if (_life > _lifetime) {
		remove = true;
	}
}