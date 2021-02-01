#ifndef MELEEATTACKBHV_H
#define MELEEATTACKBHV_H

#include <memory>
#include "behaviour.h"

class MeleeAttackBhv : public Behaviour
{
public:
	MeleeAttackBhv(Enemy* owner);
	
	virtual void collision(float x, float y, float z);
	virtual void damage(float damage);
	virtual void update(double dt);
    
    float minAttackDistance = 1.5f;
    float attackDamage = 10.0f;
    float attackCooldown = 1.0f;
    
private:
    float _attackTime = 0.0f;
};

#endif // MELEEATTACKBHV_H
