#include "enemy.h"

#include <utility>
#include "resources/resources.h"
#include "behaviour/emptybhv.h"

Enemy::Enemy(const TextureAtlas* atlas, const std::string& skin, float x, float y, float z):
	VisualEntity(x, y, z),
	_health(1.0f),
	_maxSpeed(2.0f), _minSpeed(1.0f),
	_behaviour(nullptr),
	_skin { atlas->animationList().at(skin) },
	_skinName { skin },
	_animationType(AnimationType::IDLE),
	_animation(&_skin.at(_animationType)),
	_atlas { atlas },
	_state(ES_IDLE)
{
	physics = true;
	
	Collider.Shape = ColliderInfo::ShapeCylinder;
	Collider.Mask = ColliderInfo::MaskGeometry | ColliderInfo::MaskRays;
}

Enemy::~Enemy()
{

}

void Enemy::damage(float val, const vec3& source)
{
	Entity::damage(val, source);
    
    if(_state == ES_DEATH)
        return;
    
    if(health() - val <= 0) {
        _health = 0;
        state(ES_DEATH);
    }
    else {
        _health = std::min(0.0f, _health - val);
    }
    
    vec3 here(x(), y(), z());
    
    auto dir = (here - source).normalized();
    
    knock(dir.x*5.0f, dir.y*5.0f, 2.0f);
}

void Enemy::update(float dt)
{
	Entity::update(dt);
    
    _updateState();

	_time += dt;
	if(_time > frameTime) {
		_time -= frameTime;
		
		_frame++;
	}
}

void Enemy::state(EnemyState newstate)
{
    if(_state == newstate)
        return;
    
    switch(newstate) {
        case ES_IDLE: {
            _animationType = AnimationType::IDLE;
            _animation = &_skin.at(_animationType);
        } break;
        case ES_MOVE: {
            _animationType = AnimationType::WALK;
            _animation = &_skin.at(_animationType);
        } break;
        case ES_ATTACK: {
            _animationType = AnimationType::ATTACK;
            _animation = &_skin.at(_animationType);
        } break;
        case ES_DEATH: {
            _animationType = AnimationType::DEATH;
            _animation = &_skin.at(_animationType);
            
            _behaviour = std::shared_ptr<Behaviour>(new EmptyBhv(this));
			
			_health = 0;
			_speedX = 0;
			_speedY = 0;
			_frame = 0;
			_time = 0;
			
			Collider.Mask &= ~ColliderInfo::MaskRays;
        } break;
    }
    
    _state = newstate;
}

void Enemy::_updateState()
{
    switch(_state) {
        case ES_DEATH: {
            return;
        } break;
        case ES_ATTACK: {
            if((_speedX != 0) || (_speedY != 0))
                state(ES_MOVE);
        } break;
        case ES_MOVE: {
            if((_speedX == 0) && (_speedY == 0))
                state(ES_IDLE);
        } break;
        case ES_IDLE: {
            if((_speedX != 0) || (_speedY != 0))
                state(ES_MOVE);
        } break;
    }
}

const Texture& Enemy::frame(float viewX, float viewY) const
{
	return _atlas->texture(frameID(viewX, viewY));
}

uint32_t Enemy::frameID(float viewX, float viewY) const
{
	AnimationViewDir dr = AnimationViewDir::FRONT;
	
	if(_animation->directional &&
		_animationType != AnimationType::ATTACK &&
		_animationType != AnimationType::DEATH)
		dr = viewSide(viewX, viewY);
	
	uint32_t frame_offset = _frame % (_animation->endFrame[dr] - _animation->startFrame[dr] + 1);
	
	if(_animationType == AnimationType::DEATH) {
		if(_frame > _animation->endFrame[dr] - _animation->startFrame[dr]) {
			frame_offset = _animation->endFrame[dr] - _animation->startFrame[dr];
		}
	}
	
	uint32_t id = _animation->startFrame[dr] + frame_offset;
	return id;
}

