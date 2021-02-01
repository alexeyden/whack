#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "SDL_touch.h"

class Controller
{
public:
	virtual bool onMouseReleased(double x, double y, int button, int mods) = 0;
	virtual bool onMousePressed(double x, double y, int button, int mods) = 0;
	virtual void onMouseMove(double prevX, double prevY, double x, double y, int buttons) = 0;
	
	virtual bool onKeyPressed(int key, int mods) = 0;
	virtual bool onKeyReleased(int key, int mods) = 0;
	virtual bool onCharEntered(unsigned int ch) = 0;
    
    virtual void onFingerPressed(int id, int x, int y) = 0;
    virtual void onFingerReleased(int id, int x, int y) = 0;
    virtual void onFingerMove(int id, int x, int y, int px, int py) = 0;
};

#endif // CONTROLLER_H
