#ifndef APPLICATION_H
#define APPLICATION_H

#include <cstring>
#include <map>
#include <string>

#include "graphics/texture.h"
#include "graphics/font.h"
#include "graphics/screen.h"

#include "resources/resources.h"
#include "backend/backend.h"

struct UIStyle;
class InputManager;

class Application
{
private:
	Application(unsigned width, unsigned height);
	~Application();
	
public:
	void run();
	
	void quit() {
		_quitPending = true;
	}
	
	bool limitFPS;
	
	UIStyle* globalStyle() {
		return _uiStyle;
	}

	void setScreen(Screen* sc) {
        if(_screen != nullptr)
            _pendingDelete.push_back(_screen);
        
		_screen = sc;
	}
	
	Screen* screen() {
		return _screen;
	}
	
	bool debug;
	
	bool vsync() {
		return _backend->vsync();
	}
	
	bool setVSync(bool use) {
		_backend->setVsync(use);
        return true;
	}
	
	Texture* buffer() { return _buffer; }
	Backend* backend() { return _backend; }
	
public:
	static Application* init(unsigned width, unsigned height);
	static void destroy();
	
	static Application* I() {
		return _instance;
	}
	
private:	
	static Application* _instance;

private:	
	void render(float dt);
	float updateTime();

	static void loadResources();
    
private:
    std::vector<Screen*> _pendingDelete;
	Backend* _backend;
	Screen* _screen;
	Texture* _buffer;
	UIStyle* _uiStyle;
	
	Font* _debugFont;
	
	bool _quitPending;
	
	float lastTime;
	
	float fpsAvg;
	float fpsSum;
	int fpsIter; 
};

typedef Application APP;

#endif // APPLICATION_H
