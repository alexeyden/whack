#ifndef STATICSKYDOME_H
#define STATICSKYDOME_H

#include "skydome.h"

#include "graphics/texture.h"
#include <string>

class StaticSkydome : public SkyDome
{
public:
	StaticSkydome(const std::string& name);
	
	virtual void update(float dt);
	virtual void color(unsigned char k, unsigned char* bgr) const;
	virtual void render(int x, const std::vector< Range< int > >& parts, float dir, Texture* out) const;
	
private:
	const Texture& _texture;
};

#endif // STATICSKYDOME_H
