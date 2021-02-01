#ifndef ROBBERBHV_H
#define ROBBERBHV_H

#include "behaviour.h"
#include <memory>

class RobberBhv : public Behaviour
{
public:
	RobberBhv(Enemy* owner);
	virtual void update(double dt);
	virtual void damage(float damage);
	virtual void collision(float x, float y, float z);
	
private:
	std::shared_ptr<Behaviour> _currentBhv;
	
	std::shared_ptr<Behaviour> _idleBehaviour;
	std::shared_ptr<Behaviour> _seekBehaviour;
	std::shared_ptr<Behaviour> _attackBehaviour;
	
	float _time;
	static constexpr float c_update_tick = 0.1;
};

#endif // ROBBERBHV_H
