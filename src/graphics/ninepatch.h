#ifndef NINEPATCH_H
#define NINEPATCH_H

#include "texture.h"

class TextureAtlas;

enum NinePatchPart {
	NP_PART_TOPLEFT = 0,
	NP_PART_TOP = 1,
	NP_PART_TOPRIGHT = 2,
	NP_PART_RIGHT = 3,
	NP_PART_BOTTOMRIGHT = 4,
	NP_PART_BOTTOM = 5,
	NP_PART_BOTTOMLEFT = 6,
	NP_PART_LEFT = 7,
	NP_PART_CENTER = 8
};
	
struct NinePatch
{
	NinePatch(TextureAtlas* atlas, unsigned startID);
	NinePatch(TextureAtlas* atlas, unsigned tl, unsigned t, unsigned tr, unsigned r,
						unsigned br, unsigned b, unsigned bl, unsigned l, unsigned c);
	
	const Texture topLeft;
	const Texture top;
	const Texture topRight;
	const Texture right;
	const Texture bottomRight;
	const Texture bottom;
	const Texture bottomLeft;
	const Texture left;
	const Texture center;
};

#endif // NINEPATCH_H
