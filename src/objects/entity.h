#ifndef ENTITY_H
#define ENTITY_H

#include "util/math.h"
#include "util/aabb.h"

#include "entityvisitor.h"
#include "collider.h"

class Level;

class Entity
{
public:
	Entity(float x, float y, float z) :
        level(nullptr), remove(false), physics(false),
        _falling(false), _width(1), _height(0.8),
		_x(x), _y(y), _z(z), _dir(0.0f), 
		_speedX(0.0f), _speedY(0.0f), _speedZ(0.0f) {}
		
	virtual ~Entity() {}
	
	virtual void update(float dt) {
		_x += _speedX * dt;
		_y += _speedY * dt;
		_z += _speedZ * dt;
	}
	
	virtual bool collision(float x, float y, float z) {
        (void) x; (void) y; (void) z;
        return false;
    }
	virtual void damage(float damage, const vec3& source) {
        (void) damage;
        (void) source;
    }
	
	virtual void x(float nx) { _x = nx; }
	virtual void y(float ny) { _y = ny; }
	virtual void z(float nz) { _z = nz; }
	
	virtual float x() const { return _x; }
	virtual float y() const { return _y; }
	virtual float z() const { return _z; }
	
	virtual AABB<float> collisionAABB() const {
		return AABB<float>(
			_x - _width/2.0f, _y - _width/2.0f, _z,
			_x + _width/2.0f, _y + _width/2.0f, _z + _height
		);
	}
	
	float dir() const { return _dir; }
	void dir(float newval) { _dir = newval; }
	
	float speedX() const { return _speedX; }
	float speedY() const { return _speedY; }
	float speedZ() const { return _speedZ; }
	
	virtual void speedXY(float x, float y) { _speedX = x; _speedY = y; }
	void speedZ(float newval) { _speedZ = newval; }
	
	void knock(float x, float y, float z) {
        _speedX = x;
        _speedY = y;
        _speedZ = z;
        impulse = true;
    }
	
	bool falling() const { return _falling; }
	void falling(bool falling) { _falling = falling; }
	
	float height() const  { return _height; }
	void height(float newval) { _height = newval; } 
	float width() const { return _width; }
	void width(float newval) { _width = newval;}
	
	virtual void accept(EntityVisitor* visitor) = 0;
	
	Level* level;
	bool remove;
	bool physics;
    bool impulse;
	
	ColliderInfo Collider;
    
protected:
	bool _falling;
	float _width, _height;
	float _x, _y, _z, _dir;
	float _speedX, _speedY, _speedZ;
};

#endif // ENTITY_H
