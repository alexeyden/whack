#ifndef SKYDOME_H
#define SKYDOME_H

#include <vector>

#include "util/rangeset.h"
#include "util/math.h"

class Texture;

class SkyDome
{
public:
	virtual void render(int x, const std::vector< Range< int > >& parts, float dir, Texture* out) const = 0;
	
	virtual void color(unsigned char k, unsigned char* bgr) const = 0;
	
	virtual void update(float dt) = 0;
};

#endif // SKYDOME_H
