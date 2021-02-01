#include "meleeattackbhv.h"

#include "util/math.h"
#include "objects//enemy.h"
#include "level/level.h"

MeleeAttackBhv::MeleeAttackBhv(Enemy* owner):
    Behaviour(owner),
    _attackTime { 0.0f }
{
	
}

void MeleeAttackBhv::collision(float x, float y, float z)
{
    (void) x;
    (void) y;
    (void) z;
}

void MeleeAttackBhv::damage(float damage)
{
    (void) damage;
}

void MeleeAttackBhv::update(double dt)
{
    if(_attackTime > .0f)
        _attackTime -= dt;
    
    auto player = owner->level->player();
    
    vec2 delta(player->x() - owner->x(), player->y() - owner->y());
    
    if(delta.length() < minAttackDistance) {
        owner->speedXY(.0f, .0f);
        
        if(_attackTime <= .0f) {
            player->damage(attackDamage, vec3(owner->x(), owner->y(), owner->z()));
            _attackTime = attackCooldown;
        }
        
        if(owner->state() != EnemyState::ES_ATTACK)
            owner->state(EnemyState::ES_ATTACK);
    } else {
        delta.normalize();
        delta *= owner->maxSpeed();
        
        owner->dir(atan2(delta.y, delta.x));
        owner->speedXY(delta.x, delta.y);
    }    
}
