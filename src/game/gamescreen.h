#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include "controller/controller.h"
#include "graphics/screen.h"
#include "ui/uijoypad.h"
#include "ui/uiimagebutton.h"
#include "util/math.h"

class Level;
class Renderer;
class Font;
class HUD;

class GameScreen : public Screen, public Controller
{
public:
	GameScreen();
	~GameScreen();
	
	virtual void render(Texture* out) override;
	virtual void update(float dt) override;	
	
	virtual bool onKeyPressed(int key, int mods) override;
	virtual bool onKeyReleased(int key, int mods) override;
	virtual bool onCharEntered(unsigned int ch) override {
        (void) ch;
        return false;
    }
	
	virtual void onMouseMove(double prevX, double prevY, double x, double y, int buttons) override;
	virtual bool onMousePressed(double x, double y, int button, int mods) override;
	virtual bool onMouseReleased(double x, double y, int button, int mods) override;
    
    virtual void onFingerPressed(int num, int x, int y) override;
    virtual void onFingerReleased(int num, int x, int y) override;
	virtual void onFingerMove(int num, int x, int y, int px, int py) override;
    
private:
    int _fingerX, _fingerY;
    int _fingerID;
    
	bool _movingForward, _movingBackward, _movingLeft, _movingRight;
	bool _turningLeft, _turningRight;
	
	float _time;
    
    UIJoyPad* _joypad;
    UIImageButton* _exitButton;
    UIImageButton* _jumpButton;
    UIManager* _uiManager;
	
	Renderer* _levelRenderer;
	HUD* _hud;
	Level* _level;
};

#endif // GAMESCREEN_H
