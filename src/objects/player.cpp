#include "player.h"

#include <cmath>
#include "util/util.h"

Player::Player(float x, float y, float z):
	Entity(x, y, z), 
	_health(100),
	_landingAnimDur(0.3f), dz(0), t(0)
{
	physics = true;
	Collider.Mask = ColliderInfo::MaskRays | ColliderInfo::MaskGeometry | ColliderInfo::MaskEntities;
	Collider.Shape = Collider.ShapeCylinder;
	
	_gunManager = new GunManager(this);
    _gunManager->gun().ammo = 99;
	
	_prevSpeedZ = 0.0f;
	_landingAnimTime = 0.0f;
}

Player::~Player()
{
	delete _gunManager;
}

void Player::jump()
{
	if(fabs(_speedZ) < 0.001) {
		speedZ(3.0f);
	}
}

void Player::update(float dt)
{
	Entity::update(dt);
	_gunManager->update(dt);
	
	bool moving = _speedX != 0 || _speedY != 0;
	
	if(_prevSpeedZ < 0 && _speedZ == 0.0f && !moving) { // jump landing
		_landingAnimTime = _landingAnimDur;
	}
	
	if (_landingAnimTime > 0) {
		_landingAnimTime -= dt;
		if(_landingAnimTime <= 0 || moving) {
			_landingAnimTime = 0.0f;
		}
	}
	
	if((moving && _speedZ == 0) || _landingAnimTime > 0) { //bobbing
		dz = 0.05f * sin(t * 15.0f);
	}
	else if(dz != 0) { //finish bobbing phase when stopped
		float delta = -copysign(0.5 * dt, dz);
		
		if(std::signbit(dz + delta) != std::signbit(dz))
			dz = 0;
		else
			dz += delta;
	}
	
	_prevSpeedZ = _speedZ;
	
	t += dt;
}

void Player::damage(float damage, const vec3& source) {
    if(this->_health > 0)
        this->_health -= int(damage);
}
