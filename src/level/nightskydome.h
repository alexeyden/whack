#ifndef NIGHTSKYDOME_H
#define NIGHTSKYDOME_H

#include <vector>
#include <utility>

#include "skydome.h"

class NightSkydome : public SkyDome
{
public:
	NightSkydome();
	~NightSkydome();
	
	virtual void render(int x, const std::vector< Range< int > >& parts, float dir, Texture* out) const;
	virtual void update(float dt);
	
	virtual void color(unsigned char k, unsigned char* bgr) const; 
	
private:
	int _stars[60];
	
	const unsigned _bottomColor = 0x081c35;
	const unsigned _topColor = 0x00000000;
};

#endif // NIGHTSKYDOME_H
