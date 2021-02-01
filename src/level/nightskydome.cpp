#include "nightskydome.h"

#include <cstdlib>
#include <cmath>

#include "graphics/texture.h"
#include "util/util.h"

NightSkydome::NightSkydome()
{
	for(int i = 0; i < 60; i++) {
		_stars[i] = rand() % 200 - 100;
	}
}

NightSkydome::~NightSkydome()
{

}

void NightSkydome::render(int x, const std::vector< Range< int > >& parts, float dir, Texture* out) const
{
    (void) dir;
    
	for(const Range<int>& r : parts) {
		for(int y = r.start; y <= r.end; y++) {
			unsigned char k = (y << 8)/out->height();
			
			unsigned char bgr[3];
			color(k, bgr);
			out->setPixelFastInvY(x, y, bgr[2], bgr[1], bgr[0]);
		}
	}
}

void NightSkydome::update(float dt)
{
    (void) dt;
}

void NightSkydome::color(unsigned char k, unsigned char* bgr) const
{
	bgr[2] = (colorB(_bottomColor) * (0xff + 1 - k) + k * colorB(_topColor)) >> 8;
	bgr[1] = (colorG(_bottomColor) * (0xff + 1 - k) + k * colorG(_topColor)) >> 8;
	bgr[0] = (colorR(_bottomColor) * (0xff + 1 - k) + k * colorR(_topColor)) >> 8;
}
