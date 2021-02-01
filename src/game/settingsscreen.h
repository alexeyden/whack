#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include "graphics/screen.h"

class UIManager;

class SettingsScreen : public Screen
{
public:
	SettingsScreen();
	~SettingsScreen();
	
	virtual void render(Texture* out);
	virtual void update(float dt);
	
private:
	UIManager* _ui;
	Texture _bgTexture;
};

#endif // SETTINGSSCREEN_H
