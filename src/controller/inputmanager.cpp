#include "inputmanager.h"

#include <cmath>
#include <algorithm>

std::vector<Controller*>  InputManager::_controllers;
std::map<unsigned, unsigned> InputManager::_keymap;

InputManager* InputManager::_instance = nullptr;

#include "SDL.h"
#include "SDL_log.h"

void InputManager::init(Backend* be)
{
    _keymap = {
        {SDLK_w, InputManager::FORWARD},
        {SDLK_s, InputManager::BACKWARD},
        {SDLK_a, InputManager::STRAFE_LEFT},
        {SDLK_d, InputManager::STRAFE_RIGHT},
        {SDLK_LEFT, InputManager::TURN_LEFT},
        {SDLK_RIGHT, InputManager::TURN_RIGHT},
        {SDLK_LCTRL, InputManager::FIRE},
        {SDLK_SPACE, InputManager::JUMP},
        {SDLK_F1, InputManager::DEBUG_1},
        {SDLK_F2, InputManager::DEBUG_2},
        
        {SDLK_ESCAPE, InputManager::K_ESCAPE},
        {SDLK_BACKSPACE, InputManager::K_BACKSPACE},
        {SDLK_RETURN, InputManager::K_ENTER}
    };
    
    _instance = new InputManager();
    
    be->setController(_instance);
}

void InputManager::destroy()
{
	_controllers.clear();
    
    delete _instance;
}

void InputManager::addController(Controller* controller)
{
	_controllers.push_back(controller);
}

void InputManager::removeController(Controller* controller)
{
	auto iter = std::find(_controllers.begin(), _controllers.end(), controller);
	if(iter != _controllers.end())
		_controllers.erase(iter);
}

bool InputManager::onCharEntered(unsigned int ch)
{
	for(Controller* c : _controllers) {
		if(c->onCharEntered(ch))
            break;
	}
	
	return false;
}

bool InputManager::onKeyPressed(int key, int mods)
{
    for(Controller* c : _controllers) {
        auto mapped = _keymap.find(key);
        
        if(mapped == _keymap.end())
            continue;
        
        if(c->onKeyPressed(mapped->second, mods))
            break;
    }
    
    return false;
}

bool InputManager::onKeyReleased(int key, int mods)
{
    for(Controller* c : _controllers) {
        auto mapped = _keymap.find(key);
        
        if(mapped == _keymap.end())
            continue;
        
        if(c->onKeyReleased(mapped->second, mods))
            break;
    }
    
    return false;
}

void InputManager::onMouseMove(double prevX, double prevY, double x, double y, int buttons)
{
    (void) prevX;
    (void) prevY;
    
    for(Controller* c : _controllers)
		c->onMouseMove(prevX, prevY, x, y, buttons);
}

bool InputManager::onMousePressed(double x, double y, int button, int mods)
{
    for(Controller* c : _controllers)
        if(c->onMousePressed(x, y, button, mods))
            break;
        
    return false;
}

bool InputManager::onMouseReleased(double x, double y, int button, int mods)
{
    for(Controller* c : _controllers)
        if(c->onMouseReleased(x, y, button, mods))
            break;
        
    return false;
}

void InputManager::onFingerMove(int id, int x, int y, int px, int py)
{
    for(Controller* c : _controllers)
        c->onFingerMove(id, x, y, px, py);
}

void InputManager::onFingerPressed(int id, int x, int y)
{
    for(Controller* c : _controllers)
        c->onFingerPressed(id, x, y);
}

void InputManager::onFingerReleased(int id, int x, int y)
{
    for(Controller* c : _controllers)
		c->onFingerReleased(id, x, y);
}

