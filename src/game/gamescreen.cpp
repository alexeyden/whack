#include "gamescreen.h"

#include <cstring>
#include <cmath>

#include "SDL_log.h"

#include "backend/rwops_stream.h"

#include "controller/inputmanager.h"

#include "level/renderer.h"
#include "level/level.h"
#include "level/levelloader.h"
#include "level/levelraycaster.h"
#include "level/lightingcalculator.h"

#include "objects/player.h"
#include "objects/decoration.h"
#include "objects/robber.h"

#include "graphics/font.h"
#include "graphics/renderer2d.h"

#include "ui/hud.h"
#include "util/util.h"
#include "util/math.h"

#include "application.h"
#include "menuscreen.h"

GameScreen::GameScreen()
	: _movingForward(false), _movingBackward(false), _movingLeft(false), _movingRight(false),
		_turningLeft(false), _turningRight(false),
		_time(0.0f)
{
	SDL_Log("Initializing game screen");
	
    auto uiAtlas = Resources::i()->atlas("ui");
    
    _uiManager = new UIManager(Application::I()->globalStyle());
    
    auto height = Application::I()->backend()->viewHeight();
    auto width = Application::I()->backend()->viewWidth();
    
    const auto& joypadBack = &uiAtlas->textureByName("joy_back");
    const auto& joybadStickN = &uiAtlas->textureByName("joy_front_n");
    const auto& joybadStickP = &uiAtlas->textureByName("joy_front_p");
    
    _joypad = new UIJoyPad(_uiManager, joypadBack, joybadStickN, joybadStickP); 
    _joypad->bounds = _joypad->minBounds(); 
    _joypad->bounds.moveTo(20, height - 100);
    
    _exitButton = new UIImageButton(_uiManager, &uiAtlas->textureByName("exit_n"), &uiAtlas->textureByName("exit_p"));
    _exitButton->bounds = _exitButton->minBounds(); 
    _exitButton->bounds.moveTo(width - _exitButton->bounds.width() - 10, 10);
    _exitButton->addClickHandler([]() {
        APP::I()->setScreen(new MenuScreen());
    });
    
    _jumpButton = new UIImageButton(_uiManager, &uiAtlas->textureByName("jump_n"), &uiAtlas->textureByName("jump_p"));
    _jumpButton->bounds = _jumpButton->minBounds(); 
    _jumpButton->bounds.moveTo(_jumpButton->bounds.width() + 30, height - _jumpButton->bounds.height() - 10);
    _jumpButton->addClickHandler([this]() {
        _level->player()->jump();
    });
    
    rwops_stream file("levels/demo_light.level", "rb");
	LevelLoader loader(file, Resources::i()->atlas("tiles2"), Resources::i()->atlas("entities"), Application::I()->backend());
	_level = loader.load();
    
	_levelRenderer = new Renderer(_level,
		Resources::i()->atlas("tiles2"),
		APP::I()->backend()->viewWidth(),
        APP::I()->backend()->viewHeight()
	);
	
	_hud = new HUD(_level);
    
    _fingerID = -1;
    
	InputManager::addController(this);
}

GameScreen::~GameScreen()
{
	SDL_Log("Destroying game screen");
    
	InputManager::removeController(this);
	
	delete _levelRenderer;
	delete _hud;
	delete _level;
    
    delete _joypad;
    delete _exitButton;
    delete _jumpButton;
    delete _uiManager;
}

void GameScreen::render(Texture* out)
{
	_levelRenderer->render(out);

	if(Application::I()->backend()->hasTouchscreen()) {
    	_joypad->render(out);
    	_exitButton->render(out);
    	_jumpButton->render(out);
    }

	_hud->render(out);
}

void GameScreen::update(float dt)
{
	Player* p = _level->player();
	
	vec2 speed(0.0, 0.0); 
	auto jf = _joypad->factor();
    
	const float run_speed = p->runSpeed;
	
	if(_movingForward || jf.y < -0.2f) {
		speed.x = cos(p->dir()) * run_speed;
		speed.y = sin(p->dir()) * run_speed;
	}
	else if(_movingBackward || jf.y > 0.2f) {
		speed.x = -cos(p->dir()) * p->backSpeed;
		speed.y = -sin(p->dir()) * p->backSpeed;
	}
	
	if(_movingLeft || jf.x < -0.2f) {
		speed.x += cos(p->dir() + M_PI/2.0f) * p->strafeSpeed;
		speed.y += sin(p->dir() + M_PI/2.0f) * p->strafeSpeed;
	}
	else if(_movingRight || jf.x > 0.2f) {
		speed.x += cos(p->dir() - M_PI/2.0f) * p->strafeSpeed;
		speed.y += sin(p->dir() - M_PI/2.0f) * p->strafeSpeed;
	}
	
	if(speed.length() > run_speed) {
		speed.normalize();
		speed *= 0.6 * run_speed + 0.4 * p->strafeSpeed;
	}
    
	if(!p->falling()) {
		p->speedXY(speed.x, speed.y);
	}
	
	if(_turningLeft) {
		p->dir(p->dir() + p->turnSpeed * dt);
	}
	else if(_turningRight) {
		p->dir(p->dir() - p->turnSpeed * dt);
	}
	
	_level->update(dt);
	_levelRenderer->update();
    _hud->update(dt);
	
	_time += dt;
	
	_levelRenderer->drawEntityCells = APP::I()->debug;
}

bool GameScreen::onKeyPressed(int key, int mods)
{
    (void) mods;
    
	switch(key) {
        case InputManager::DEBUG_1: {
			APP::I()->debug ^= 1;
		} break;
        case InputManager::FORWARD: _movingForward = true; break;
		case InputManager::BACKWARD: _movingBackward = true; break;
		case InputManager::STRAFE_LEFT: _movingLeft = true; break;
		case InputManager::STRAFE_RIGHT: _movingRight = true; break;
		case InputManager::JUMP: {
			_level->player()->jump();
		} break;
		case InputManager::TURN_LEFT: _turningLeft = true; break;
		case InputManager::TURN_RIGHT: _turningRight = true; break;
        case InputManager::FIRE: {
			_level->player()->gunManager().fire();
		} break;
        case InputManager::K_ESCAPE: {
			APP::I()->setScreen(new MenuScreen());
			break;
		}
		default: return false;
	}
	
	return true;
}

bool GameScreen::onKeyReleased(int key, int mods)
{
    (void) mods;
    
	switch(key) {
        case InputManager::FORWARD: _movingForward = false; break;
		case InputManager::BACKWARD: _movingBackward = false; break;
		case InputManager::STRAFE_LEFT: _movingLeft = false; break;
		case InputManager::STRAFE_RIGHT: _movingRight = false; break;
		case InputManager::TURN_LEFT: _turningLeft = false; break;
		case InputManager::TURN_RIGHT: _turningRight = false; break;
		default: break;
	}
	
	return true;
}

void GameScreen::onMouseMove(double prevX, double prevY, double x, double y, int buttons) {
    (void) prevX;
    (void) prevY;
    (void) x;
    (void) y;
    (void) buttons;
}


bool GameScreen::onMousePressed(double x, double y, int button, int mods) {
    (void) x;
    (void) y;
    (void) button;
    (void) mods;
    
    return false;
}


bool GameScreen::onMouseReleased(double x, double y, int button, int mods) {
    (void) x;
    (void) y;
    (void) button;
    (void) mods;
    
    return false;
}

void GameScreen::onFingerPressed(int num, int x, int y)
{
	if(!Application::I()->backend()->hasTouchscreen())
		return;

    if(_joypad->bounds.inside(x,y)) {
        _joypad->onFingerPressed(num, x,y);
    }
    else if(_exitButton->bounds.inside(x,y)) {
        _exitButton->onFingerPressed(num, x, y);
    }
    else if(_jumpButton->bounds.inside(x,y)) {
        _jumpButton->onFingerPressed(num, x, y);
    }
    else if(_fingerID == -1) {
        _fingerID = num;
        _fingerX = x;
        _fingerY = y;
    }
}

void GameScreen::onFingerReleased(int num, int x, int y)
{
    if(!Application::I()->backend()->hasTouchscreen())
        return;

    _joypad->onFingerReleased(num, x, y);
    _exitButton->onFingerReleased(num, x, y);
    _jumpButton->onFingerReleased(num, x, y);
    
    if(num == _fingerID) {
        if(((_fingerX - x)*(_fingerX - x) + (_fingerY - y)*(_fingerY - y)) < 10) {
            _level->player()->gunManager().fire();
        }
        _fingerID = -1;
    }
}

void GameScreen::onFingerMove(int num, int x, int y, int px, int py)
{
    if(!Application::I()->backend()->hasTouchscreen())
        return;

    _joypad->onFingerMove(num, x, y, px, py);
    _exitButton->onFingerMove(num, x, y, px, py);
    _jumpButton->onFingerMove(num, x, y, px, py);
    
    if(num == _fingerID) {
        Player* p = _level->player();
        p->dir(p->dir() - (x - px)*0.01f);
    }
}
