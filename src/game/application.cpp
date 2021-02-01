#include "application.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>

#include "controller/inputmanager.h"
#include "ui/uistyle.h"
#include "util/util.h"

#include "backend/backend.h"

Application* Application::_instance = nullptr;

Application* Application::init(unsigned int width, unsigned int height)
{
    _instance = new Application(width, height);
    
	SDL_Log("Loading resources..");
	
    loadResources();
    
	return _instance;
}

void Application::destroy()
{
	delete _instance;
	_instance = nullptr;
}

Application::Application(unsigned int width, unsigned int height) :
	debug(false), _screen(nullptr), _quitPending(false)
{
	SDL_Log("Initializing backend..");
	
	_backend = new Backend("Whack", width, height);
    
	InputManager::init(_backend);
    Resources::init(_backend);
	
	uint8_t* data = new uint8_t[512 * 512 * 3];
	_buffer = new Texture(data, 0, 0,
                          _backend->viewWidth(), _backend->viewHeight(),
                          _backend->viewWidth(), _backend->viewHeight());
	limitFPS = false;

	srand(time(nullptr));
}

Application::~Application()
{
	SDL_Log("Shutting down..");
    
	delete _buffer;
	delete _debugFont;
	
	InputManager::destroy();

	delete _backend;
    
	SDL_Log("Farewell");
}

void Application::loadResources()
{
	Resources::i()->loadAtlas("textures/font.bmp", "font");
	Resources::i()->loadAtlas("textures/entities.json", "entities");
	Resources::i()->loadAtlas("textures/ui.json", "ui");
	Resources::i()->loadAtlas("textures/sheet.json", "tiles");
	Resources::i()->loadAtlas("textures/tiles.json", "tiles2");
	Resources::i()->loadAtlas("textures/skydomes.json", "skydomes");
	
	_instance->_debugFont = new Font(&Resources::i()->atlas("font")->textureList()[0], 8);
	
	_instance->_uiStyle = new UIStyle {
		Font(&Resources::i()->atlas("font")->asTexture(), 8, 0x004656B9, 0x00061367, 0x0000093F, 0, 2), //font normal
		Font(&Resources::i()->atlas("font")->asTexture(), 8, 0x00202F90, 0x00000000, 0x00, 0, 0), //font active
		Font(&Resources::i()->atlas("font")->asTexture(), 8, 0x00203090, 0x00000000, 0x00, 0, 0), //font disabled 
		
		0x00202F90, //background normal
		0x00061367, //background active
		0x00101450, //background disabled
		
		Resources::i()->atlas("ui")->ninePatch("button_normal"), //button normal 9patch
		Resources::i()->atlas("ui")->ninePatch("button_pressed"), //button pressed 9patch
		
		Resources::i()->atlas("ui")->textureByName("checkbox_off"), //checkbox unchecked image
		Resources::i()->atlas("ui")->textureByName("checkbox_on") //checkbox unchecked image
	};
}

void Application::render(float dt)
{
    (void) dt;
    
	_buffer->clearBlack();
	
	if(_screen != nullptr)
		_screen->render(_buffer);
	
	if(debug) {
		char buf[64];
		
		snprintf(buf, 30, "%s: %0.2f", "FPS", fpsAvg);
		_debugFont->drawText(_buffer, _backend->viewWidth() - 100, 8, buf);
	}
}

void Application::run()
{
	lastTime = _backend->getTime();
	fpsAvg = 0.0f;
	fpsIter = 0;

	SDL_Log("Starting game loop");
    
	do{
		if(!limitFPS || !(_backend->getTime() - lastTime < 1/30.0))
		{	
            while(_pendingDelete.size() > 0) {
                delete _pendingDelete[0];
                _pendingDelete.erase(_pendingDelete.begin());
            }
            
			float dt = updateTime();
	
			if(_screen != nullptr)
				_screen->update(dt);
			
			render(dt);
			_backend->draw(_buffer);
		}
	} 
	while(_backend->pumpEvents() == false && _quitPending == false);
}

float Application::updateTime()
{
	double t = _backend->getTime();
	double dt = t - lastTime;
	lastTime = t;
	
	fpsSum += 1/dt;
	fpsIter++;
	if(fpsIter > 10) {
		fpsAvg = fpsSum/float(fpsIter);
		fpsSum = 0;
		fpsIter = 0;
	}
	
	return dt;
}
