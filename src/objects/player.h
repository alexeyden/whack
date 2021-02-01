#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "gunmanager.h"

class Player : public Entity
{
public:
	Player(float x, float y, float z);
	virtual ~Player();
	
	virtual void update(float dt);
	
	float viewZ() const { return _z  + _height/2 + dz; }
	
	void jump();
	
	virtual void accept(EntityVisitor* visitor) { visitor->visit(this); }
	
	int health() const {
		return _health;
	}
	
	virtual void damage(float damage, const vec3& source) override;
	
	const float runSpeed = 5.0f;
	const float strafeSpeed = 2.5f;
	const float backSpeed = 2.0f;
	const float turnSpeed = 2.0f;
	
	GunManager& gunManager() {
		return *_gunManager;
	}
	
	const GunManager& gunManager() const {
		return *_gunManager;
	}
	
private:
	int _health;
	float _prevSpeedZ;
	float _landingAnimTime;
	const float _landingAnimDur; 
	
	GunManager* _gunManager;
	
	float dz;
	float t;
};

#endif // PLAYER_H
