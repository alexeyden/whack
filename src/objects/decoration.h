#ifndef DECORATION_H
#define DECORATION_H

#include "visualentity.h"
#include "entityvisitor.h"
#include "graphics/textureatlas.h"

class Decoration : public VisualEntity
{
public:
	Decoration(const TextureAtlas* atlas, const std::string& anim, float x, float y, float z);
	
	virtual const Texture& frame(float viewX, float viewY) const;
	virtual uint32_t frameID(float viewX, float viewY) const;
	
	virtual void update(float dt);
	
	const std::string& skinName() const {
		return _atlas->animationName(_animation, IDLE);
	}
	
	virtual void accept(EntityVisitor* v) { v->visit(this); }
	
private:
	float _time;
	uint32_t _frame;
	
	const Animation &_animation;
	const TextureAtlas* _atlas;
};

#endif // DECORATION_H
