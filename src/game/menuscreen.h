#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include "graphics/screen.h"
#include "ui/uimanager.h"

class TextureAtlas;

class MenuScreen : public Screen
{
public:
	MenuScreen();
	~MenuScreen();
	
	virtual void render(Texture* out);
	virtual void update(float dt);
	
private:
	UIManager* _ui;
	Texture _bgTexture;
};

#endif // MENUSCREEN_H
