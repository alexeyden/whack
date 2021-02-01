#ifndef SCREEN_H
#define SCREEN_H

#include "texture.h"

class Screen {
public:
    virtual ~Screen() {}
    
	virtual void update(float dt) = 0;
	virtual void render(Texture* out) = 0;
};

#endif
