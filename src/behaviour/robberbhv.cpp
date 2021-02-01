#include "robberbhv.h"

#include "hangaroundbhv.h"
#include "emptybhv.h"
#include "meleeattackbhv.h"

#include "objects/enemy.h"
#include "util/raytracer.h"

RobberBhv::RobberBhv(Enemy* owner) : 
	Behaviour(owner),
	_idleBehaviour(new HangAroundBhv(owner)),
	_seekBehaviour(new EmptyBhv(owner)),
	_attackBehaviour(new MeleeAttackBhv(owner))
{
	_currentBhv = _idleBehaviour;
	
	_time = 0.0f;
}

void RobberBhv::update(double dt)
{
	_time += dt;
	
	if(_time > c_update_tick) {
		_time -= c_update_tick;
        
        if(owner->impulse)
            return;
		
		EmptyEntityFilter tracerFilter;
		tracerFilter.includePlayer = true;
		RayTracer tracer(owner->level, &tracerFilter);
			
		tracer.trace(
			owner->x(), owner->y(),
			cos(owner->dir()), sin(owner->dir()), true);
		
		bool found =
			tracer.hitEntities().find(owner->level->player()) != tracer.hitEntities().end() &&
			tracer.hits()[0].end.distance > vec2(owner->level->player()->x() - owner->x(), owner->level->player()->y() - owner->y()).length();
			
		if(found) {
			_currentBhv = _attackBehaviour;
		}
		else {
			_currentBhv = _idleBehaviour;
		}
	}
	
	_currentBhv->update(dt);	
}

void RobberBhv::collision(float x, float y, float z)
{
	_currentBhv->collision(x,y,z);	
}

void RobberBhv::damage(float damage)
{
	_currentBhv->damage(damage);	
}
