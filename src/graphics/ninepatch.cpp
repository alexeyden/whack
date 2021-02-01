#include "ninepatch.h"

#include "textureatlas.h"
#include "texture.h"

NinePatch::NinePatch(TextureAtlas* atlas, unsigned int tl, unsigned int t, unsigned int tr, unsigned int r, unsigned int br,
										 unsigned int b, unsigned int bl, unsigned int l, unsigned int c)
:
topLeft(atlas->textureList()[tl]),
top(atlas->textureList()[t]),
topRight(atlas->textureList()[tr]), 
right(atlas->textureList()[r]),
bottomRight(atlas->textureList()[br]),
bottom(atlas->textureList()[b]),
bottomLeft(atlas->textureList()[bl]),
left(atlas->textureList()[l]),
center(atlas->textureList()[c]) {}

NinePatch::NinePatch(TextureAtlas* atlas, unsigned int tl)
:
topLeft(atlas->textureList()[tl]),
top(atlas->textureList()[tl + 1]),
topRight(atlas->textureList()[tl + 2]), 
right(atlas->textureList()[tl + 3]),
bottomRight(atlas->textureList()[tl + 4]),
bottom(atlas->textureList()[tl + 5]),
bottomLeft(atlas->textureList()[tl + 6]),
left(atlas->textureList()[tl + 7]),
center(atlas->textureList()[tl + 8]) {}