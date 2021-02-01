#ifndef VISUALENTITY_H
#define VISUALENTITY_H

#include "entity.h"

#include "graphics/texture.h"
#include "graphics/animation.h"

#include "util/util.h"
#include "util/rect.h"

class VisualEntity : public Entity
{
public:
	VisualEntity(float x, float y, float z) :
		Entity(x,y,z),
		lighting_static(false),
		_visualHeight(1.0f),
		_visualWidth(1.0f) {}
	
	virtual const Texture& frame(float viewX, float viewY) const = 0;
	virtual uint32_t frameID(float viewX, float viewY) const = 0;
	
	float visualHeight() const { return _visualHeight; }
	float visualWidth() const { return _visualWidth; }
	
	void visualHeight(float newval) { _visualHeight = newval; }
	void visualWidth(float newval) { _visualWidth = newval; }
	
	virtual AABB<float> visualAABB() const {
		return AABB<float>(
			_x - _visualWidth/2.0f, _y - _visualWidth/2.0f, _z,
			_x + _visualWidth/2.0f, _y + _visualWidth/2.0f, _z + _visualHeight
		);
	}
	
	uint32_t lighting_tint[3];
	bool lighting_static;
protected:
	AnimationViewDir viewSide(float vx, float vy) const;
	
	const float frameTime = 0.2f;
private:
	float _visualHeight;
	float _visualWidth;
};

#endif // VISUALENTITY_H
