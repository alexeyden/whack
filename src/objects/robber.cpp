#include "robber.h"

#include "behaviour/behaviour.h"
#include "behaviour/robberbhv.h"
#include "resources/resources.h"

Robber::Robber(const TextureAtlas* atlas, const std::string& skin, float x, float y, float z):
	Enemy(atlas, skin, x, y, z)
{
	_behaviour = std::shared_ptr<Behaviour>(new RobberBhv(this));
}

Robber::~Robber()
{
}

void Robber::update(float dt)
{
	Enemy::update(dt);
	_behaviour->update(dt);
}

bool Robber::collision(float x, float y, float z)
{
	if(state() == ES_IDLE) {
		_behaviour->collision(x, y, z);
	}
	return true;
}
