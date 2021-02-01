#ifndef COLORSKYDOME_H
#define COLORSKYDOME_H

#include <cstdint>
#include "skydome.h"

class ColorSkydome : public SkyDome
{
public:
	ColorSkydome(const uint8_t r, const uint8_t g, const uint8_t b);
	
	virtual void color(unsigned char k, unsigned char* bgr) const;
	virtual void render(int x, const std::vector< Range< int > >& parts, float dir, Texture* out) const;
	virtual void update(float dt);
	
private:
	uint8_t _color[3];
};

#endif // COLORSKYDOME_H
