#include "colorskydome.h"

#include <algorithm>
#include "graphics/texture.h"

ColorSkydome::ColorSkydome(const uint8_t r, const uint8_t g, const uint8_t b)
{
	_color[0] = b;
	_color[1] = g;
	_color[2] = r;
}

void ColorSkydome::color(unsigned char k, unsigned char* bgr) const
{
    (void) k;
    
	std::copy(_color, _color + 3, bgr);
}

void ColorSkydome::render(int x, const std::vector< Range< int > >& parts, float dir, Texture* out) const
{
    (void) dir;
    
	for(const Range<int>& r : parts) {
		for(int y = r.start; y <= r.end; y++) {
			out->setPixelFastInvY(x, y, _color[2], _color[1], _color[0]);
		}
	}
}

void ColorSkydome::update(float dt)
{
    (void) dt;
}

