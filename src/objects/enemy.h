#ifndef ENEMY_H
#define ENEMY_H

#include <memory>

#include "visualentity.h"
#include "graphics/textureatlas.h"
#include "behaviour/behaviour.h"

enum EnemyState {
	ES_IDLE = 0,
	ES_MOVE,
	ES_ATTACK,
	ES_DEATH
};

class Enemy : public VisualEntity
{
public:
	Enemy(const TextureAtlas* atlas, const std::string& skin, float x, float y, float z);
	virtual ~Enemy();
	
	virtual void update(float dt);
	virtual const Texture& frame(float viewX, float viewY) const;
	virtual uint32_t frameID(float viewX, float viewY) const;
	
	float maxSpeed() const { return _maxSpeed; }
	float minSpeed() const { return _minSpeed; }
	
	virtual void state(EnemyState newstate);
	EnemyState state() const { return _state; }
	
	virtual void damage(float val, const vec3& source) override;
	
	int health() const { return _health; }
	
	std::shared_ptr<Behaviour> behaviour() {
		return _behaviour;
	}
	
	void behaviour(Behaviour* b) {
		_behaviour = std::shared_ptr<Behaviour>(b);
	}
	
	const std::string& skinName() const {
		return _skinName;
	}
	
protected:
    void _updateState();
	
protected:
	int _health;
	float _maxSpeed;
	float _minSpeed;
	std::shared_ptr<Behaviour> _behaviour;

	const std::map<AnimationType, Animation>& _skin;
	std::string _skinName;
	AnimationType _animationType;
	const Animation* _animation;
	const TextureAtlas* _atlas;
	
	uint32_t _frame;
	float _time;
private:
	EnemyState _state;
	
};

#endif // ENEMY_H
