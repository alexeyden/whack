#ifndef UISTYLE_H
#define UISTYLE_H

#include "graphics/texture.h"
#include "graphics/ninepatch.h"
#include "graphics/font.h"

struct UIStyle {
	const Font fontNormal;
	const Font fontActivated;
	const Font fontDisabled;
	
	unsigned colorNormal;
	unsigned colorActivated;
	unsigned colorDisabled;
	
	const NinePatch buttonNormal;
	const NinePatch buttonPressed;
	
	const Texture checkboxUnchecked;
	const Texture checkboxChecked;
};

#endif // UISTYLE_H