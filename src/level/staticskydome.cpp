#include "staticskydome.h"

#include "resources/resources.h"
#include <cmath>

StaticSkydome::StaticSkydome(const std::string& name)
 : _texture(Resources::i()->atlas("skydomes")->textureByName(name))
{
}

void StaticSkydome::update(float dt)
{
    (void) dt;
}

void StaticSkydome::color(unsigned char k, unsigned char* bgr) const
{
    (void) k;
    
	const unsigned char* bgrSrc = _texture.pixelPtr(0, 0);

	bgr[2] = bgrSrc[2];
	bgr[1] = bgrSrc[1];
	bgr[0] = bgrSrc[0];
}

void StaticSkydome::render(int x, const std::vector< Range< int > >& parts, float dir, Texture* out) const
{
	for(const Range<int>& r : parts) {
		for(int y = r.start; y <= r.end; y++) {
			float ky = float(y)/out->height();
			float kx = (dir + M_PI)/(M_PI * 2);
			int xi = kx * _texture.width();
			int yi = ky * _texture.height();
			
			if(xi < 0)
				xi += _texture.width();
			
			if(xi >= (int) _texture.width())
				xi -= _texture.width();
			
			const unsigned char* bgr = _texture.pixelPtr(xi, yi);
			out->setPixelFastInvY(x, y, bgr[2], bgr[1], bgr[0]);
		}
	}
}
