#include "decoration.h"

#include "resources/resources.h"

Decoration::Decoration(const TextureAtlas* atlas, const std::string& anim, float x, float y, float z):
	VisualEntity(x, y, z),
    _time(0),
	_frame(0),
	_animation(atlas->animation(anim, AnimationType::IDLE)),
	_atlas(atlas)
{
	Collider.Mask = ColliderInfo::MaskEntities | Collider.MaskRays;
	lighting_static = true;
}

void Decoration::update(float dt)
{
	Entity::update(dt);

	_time += dt;
	if(_time > frameTime) {
		_time -= frameTime;
		
		_frame++;
	}
}

uint32_t Decoration::frameID(float viewX, float viewY) const
{
	AnimationViewDir dr = AnimationViewDir::FRONT;
	
	if(_animation.directional) {
		dr = viewSide(viewX, viewY);
	}
	
	return _animation.startFrame[dr] + _frame % (_animation.endFrame[dr] - _animation.startFrame[dr] + 1);
}


const Texture& Decoration::frame(float viewX, float viewY) const
{
	return _atlas->texture(frameID(viewX, viewY));
}
