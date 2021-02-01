#ifndef ROBBER_H
#define ROBBER_H

#include "enemy.h"
#include "entityvisitor.h"

class Robber : public Enemy
{
public:
	Robber(const TextureAtlas* atlas, const std::string& skin, float x, float y, float z);
	virtual ~Robber();
	
	virtual bool collision(float x, float y, float z);
	
	virtual void update(float dt);
	
	virtual void accept(EntityVisitor* v) { v->visit(this); }
	
};

#endif // ROBBER_H
