#ifndef SPLASH_H
#define SPLASH_H

#include "decoration.h"

class Splash : public Decoration
{
public:
	Splash(const TextureAtlas* atlas, const std::string& anim,
						 float x, float y, float z,
						float lifetime) :
						Decoration(atlas, anim, x, y, z),
						_lifetime(lifetime), _life(0)
	{
		Collider.Mask = 0;
		lighting_static = false;
	}
	
	virtual void update(float dt);
	
	virtual void accept(EntityVisitor* v) { v->visit(this); }
private:
	float _lifetime;
	float _life;
};

#endif // SPLASH_H
