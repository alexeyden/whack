#ifndef BACKEND_H
#define BACKEND_H

#include <string>
#include <cstdint>

#include <GLES2/gl2.h>

#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_log.h"

#include <utility>

class Texture;
class Controller;

class Backend
{
public:
    static constexpr uint32_t MAX_FINGERS = 3;
    
    Backend(const std::string& title, uint32_t viewWidth, uint32_t viewHeight);
    ~Backend();
    
	void draw(Texture* buffer);
	bool pumpEvents();
	double getTime();
	
	bool vsync();
	void setVsync(bool v);
    
    SDL_Thread* createThread(int (*fn)(void*), const char* name, void* data);
    int waitThread(SDL_Thread* thread);

    bool hasTouchscreen() const {
#ifdef __ANDROID__
		return true;
#else
		return false;
#endif
    }
    
    void setController(Controller* controller) {
        _controller = controller;
    }
    
    unsigned viewWidth() const {
        return _viewWidth;
    }
    
    unsigned viewHeight() const {
        return _viewHeight;
    }
    
protected:
    int getFingerNumber(SDL_FingerID id) const;
    int addFingerNumber(SDL_FingerID id);
    
    Controller* _controller;
    
    unsigned _viewWidth;
    unsigned _viewHeight;
    
    void initWindow(const std::string& title);
    void initOpenGL();
    
    std::pair<int, int> getScreenSize();
    
    SDL_FingerID _fingers[MAX_FINGERS];
    
    SDL_Window* _window;
    SDL_GLContext _context;
    
    GLuint _vboVert, _vboTex;
    GLuint _shader;
    GLuint _texture;
    GLuint _sampler;
    
    float _scale;
};

#endif // BACKEND_H
