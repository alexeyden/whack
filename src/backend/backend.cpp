#include "backend.h"

#include <cstring>

#include "controller/controller.h"
#include "graphics/texture.h"
#include "util/util.h"

Backend::Backend(const std::string& title, uint32_t viewWidth, uint32_t viewHeight) :
    _controller(nullptr)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    auto size = getScreenSize();
    
    float ratio = size.second / (float) size.first;
    viewHeight = (uint32_t) (ratio * viewWidth);
    
    SDL_Log("View Width = %d, View Height = %d", viewWidth, viewHeight);
    SDL_Log("Screen Width = %d, Screen Height = %d", size.first, size.second);
    
    _scale = viewWidth / (float) size.first;
    
    _viewWidth = viewWidth;
    _viewHeight = viewHeight;
    
    for(uint32_t i = 0; i < MAX_FINGERS; i++) {
        _fingers[i] = -1;
    }
    
	initWindow(title);
	initOpenGL();
}

Backend::~Backend()
{
    SDL_Log("Shutting down backend");
    
    glDeleteBuffers(1, &_vboVert);
    glDeleteBuffers(1, &_vboTex);
    glDeleteProgram(_shader);
    
    SDL_GL_DeleteContext(_context);
    SDL_DestroyWindow(_window);
}

void Backend::initWindow(const std::string& title)
{
    auto size = getScreenSize();
    
    _window = SDL_CreateWindow(title.c_str(), 0, 0, size.first, size.second,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
#ifdef ANDROID
    SDL_SetWindowFullscreen(_window, SDL_TRUE);
#endif
    
    _context = SDL_GL_CreateContext(_window);
}

void Backend::initOpenGL()
{    
    auto size = getScreenSize();
    
#ifdef ANDROID
    const GLchar* vertex_shader =
        "attribute highp vec3 pos;\n" \
        "attribute highp vec2 uv;\n" \
        "varying highp vec2 uv0;\n" \
        "void main(){\n" \
        "	uv0 = uv;\n" \
        "	gl_Position = vec4((pos - vec3(1.0, 1.0, 0.0)), 1.0);\n" \
        "}";

	const GLchar* fragment_shader = 
        "varying highp vec2 uv0;\n" \
        "uniform sampler2D sampler;\n" \
        "void main(){\n" \
        "	gl_FragColor = vec4(texture2D( sampler, uv0 ).bgr, 1.0);\n" \
        "}\n";
#else
     const GLchar* vertex_shader =
        "attribute vec3 pos;\n" \
        "attribute vec2 uv;\n" \
        "varying vec2 uv0;\n" \
        "void main(){\n" \
        "	uv0 = uv;\n" \
        "	gl_Position = vec4((pos - vec3(1.0, 1.0, 0.0)), 1.0);\n" \
        "}";

	const GLchar* fragment_shader = 
        "varying vec2 uv0;\n" \
        "uniform sampler2D sampler;\n" \
        "void main(){\n" \
        "	gl_FragColor = vec4(texture2D( sampler, uv0 ).bgr, 1.0);\n" \
        "}\n";    
#endif
	
    SDL_Log("Clear viewport");
        
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glViewport(0, 0, size.first, size.second);

    GLint status;
    
    SDL_Log("Create shaders");
    
	GLuint vs_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs_id = glCreateShader(GL_FRAGMENT_SHADER);
    
    SDL_Log("Compile shaders");
    
	glShaderSource(vs_id, 1, &vertex_shader, NULL);
    glCompileShader(vs_id);
    glGetShaderiv(vs_id, GL_COMPILE_STATUS, &status);
    if(!status) 
    {
        GLint infoLen = 0;
        glGetShaderiv(vs_id, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(vs_id, infoLen, NULL, infoLog);
            SDL_Log("Error compiling vs:\n%s\n", infoLog);
            delete[] infoLog;
        }
    }
	
    glShaderSource(fs_id, 1, &fragment_shader, NULL);
    glCompileShader(fs_id);
    glGetShaderiv(fs_id, GL_COMPILE_STATUS, &status);
    if(!status) 
    {
        GLint infoLen = 0;
        glGetShaderiv(fs_id, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char* infoLog = new char[infoLen];
            glGetShaderInfoLog(fs_id, infoLen, NULL, infoLog);
            SDL_Log("Error compiling fs:\n%s\n", infoLog);
            delete[] infoLog;
        }
    }
    
    SDL_Log("Link program");
    
	_shader = glCreateProgram();
	glAttachShader(_shader, vs_id);
	glAttachShader(_shader, fs_id);
    glBindAttribLocation(_shader, 0, "pos");
    glBindAttribLocation(_shader, 1, "uv");
	glLinkProgram(_shader);
    
    glGetProgramiv(_shader, GL_LINK_STATUS, &status);
    
    if(!status) 
    {
        GLint infoLen = 0;
        glGetProgramiv(_shader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1)
        {
            char* infoLog = new char[infoLen];
            glGetProgramInfoLog(_shader, infoLen, NULL, infoLog);
            SDL_Log("Error linking program:\n%s\n", infoLog);
            delete[] infoLog;
        }
    }
    
	glDeleteShader(vs_id);
	glDeleteShader(fs_id);
	
	_sampler = glGetUniformLocation(_shader, "sampler");
	
    const float fw = (float) 2.0f;
    const float fh = (float) 2.0f;
    
	static const GLfloat vertex_data[] = {
		0.0f, 0.0f, 0.0f,
		0.0f,   fh, 0.0f,
          fw,   fh, 0.0f,
		
		0.0f, 0.0f, 0.0f,
		  fw,   fh, 0.0f,
          fw, 0.0f, 0.0f
	};

    const float fvw = (float) _viewWidth;
    const float fvh = (float) _viewHeight;
    
	static const GLfloat uv_data[] = { 
		0.0f,       0.0f,
		0.0f,       fvh/512.0f,
		fvw/512.0f, fvh/512.0f,
		
		0.0f,       0.0f,
		fvw/512.0f, fvh/512.0f,
		fvw/512.0f, 0.0f
	};

    SDL_Log("Gen buffers");
    
	glGenBuffers(1, &_vboVert);
	glBindBuffer(GL_ARRAY_BUFFER, _vboVert);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

	glGenBuffers(1, &_vboTex);
	glBindBuffer(GL_ARRAY_BUFFER, _vboTex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv_data), uv_data, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, _vboVert);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, _vboTex);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	
	unsigned char* data = new unsigned char[512 * 512 * 3];
	memset(data, 0xff, 512 * 512 * 3);
    
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	delete [] data;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
}

void Backend::setVsync(bool v)
{
    (void) v;
}

bool Backend::vsync()
{
    return false;
}

void Backend::draw(Texture* buffer)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _viewWidth, _viewHeight,
                    GL_RGB, GL_UNSIGNED_BYTE, buffer->data());
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(_shader);

	glBindBuffer(GL_ARRAY_BUFFER, _vboVert);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, _vboTex);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	
	glUniform1i(_sampler, 0);

	glDrawArrays(GL_TRIANGLES, 0, 2*3);

	SDL_GL_SwapWindow(_window);
}

double Backend::getTime()
{
    return SDL_GetTicks() / 1000.0;
}

bool Backend::pumpEvents()
{
    SDL_Event event;
    bool quit = false;
    
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            quit = true;
        }
        else if(event.type == SDL_KEYDOWN) {
            if(_controller != nullptr)
                _controller->onKeyPressed(event.key.keysym.sym, event.key.keysym.mod);
        }
        else if(event.type == SDL_KEYUP) {
            if(_controller != nullptr)
                _controller->onKeyReleased(event.key.keysym.sym, event.key.keysym.mod);
        }
        else if(event.type == SDL_MOUSEBUTTONDOWN) {
            if(_controller != nullptr) {
                int bx = event.button.x;
                int by = event.button.y;
                int nx = int(bx * _scale);
                int ny = int(by * _scale);
                
                _controller->onMousePressed(nx, ny, event.button.button, 0);
                
#ifndef ANDROID
                _controller->onFingerPressed(0, nx, ny);
#endif
            }
        }
        else if(event.type == SDL_MOUSEBUTTONUP) {
            if(_controller != nullptr) {
                int bx = event.button.x;
                int by = event.button.y;
                int nx = int(bx * _scale);
                int ny = int(by * _scale);
                
                _controller->onMouseReleased(nx, ny, event.button.button, 0);
                
#ifndef ANDROID
                _controller->onFingerReleased(0, nx, ny);
#endif
            }
        }
        else if(event.type == SDL_MOUSEMOTION) {
            if(_controller != nullptr) {
                int pnx = int((event.motion.x - event.motion.xrel) * _scale);
                int pny = int((event.motion.y - event.motion.yrel) * _scale);
                int nx = int(event.motion.x * _scale);
                int ny = int(event.motion.y * _scale);
                
                _controller->onMouseMove(pnx, pny, nx, ny, event.motion.state);
                
#ifndef ANDROID
                _controller->onFingerMove(0, nx, ny, pnx, pny);
#endif
            }
        }
        else if(event.type == SDL_TEXTINPUT) {
            if(_controller != nullptr) {
                char* t = event.text.text;
                while(*t) {
                    _controller->onCharEntered(*t);
                    t++;
                }
            }
        }
        else if(event.type == SDL_FINGERUP) {
            if(_controller != nullptr) {
                int nx = int(event.tfinger.x * _viewWidth);
                int ny = int(event.tfinger.y * _viewHeight);
                int num = getFingerNumber(event.tfinger.fingerId);
                
                SDL_Log("Finger upped: %i", num);
                
                if(num != MAX_FINGERS) {
                    _fingers[num] = -1;
                    _controller->onFingerReleased(num, nx, ny);
                }
            }
        }
        else if(event.type == SDL_FINGERDOWN) {
            if(_controller != nullptr) {
                int nx = int(event.tfinger.x * _viewWidth);
                int ny = int(event.tfinger.y * _viewHeight);
                int num = addFingerNumber(event.tfinger.fingerId);
                
                SDL_Log("Finger added: %i", num);
                
                if(num != MAX_FINGERS) {
                    _controller->onFingerPressed(num, nx, ny);
                }
            }
        }
        else if(event.type == SDL_FINGERMOTION) {
            if(_controller != nullptr) {
                int nx = int(event.tfinger.x * _viewWidth);
                int ny = int(event.tfinger.y * _viewHeight);
                int dx = int(event.tfinger.dx * _viewWidth);
                int dy = int(event.tfinger.dy * _viewHeight);
                int num = getFingerNumber(event.tfinger.fingerId);
                
                
                if(num != MAX_FINGERS) {
                    _controller->onFingerMove(num, nx, ny, nx - dx, ny - dy);
                }
            }
        }
    }
    
    return quit;
}

SDL_Thread* Backend::createThread(int (*fn)(void *), const char* name, void* data)
{
    SDL_Log("Creating thread %s", name);
    
    return SDL_CreateThread(fn, name, data);
}

int Backend::waitThread(SDL_Thread* thread)
{
    SDL_Log("Awaiting thread %lu", SDL_GetThreadID((SDL_Thread*) thread));
    
    int status;
    SDL_WaitThread((SDL_Thread*) thread, &status);
    
    SDL_Log("Thread %lu exited", SDL_GetThreadID((SDL_Thread*) thread));
    
    return status;
}

std::pair<int, int> Backend::getScreenSize()
{
#ifdef ANDROID
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(0, &mode);
    return std::pair<int,int>(mode.w, mode.h);
#else
    return std::pair<int,int>(640, 480);
#endif
}

int Backend::getFingerNumber(SDL_FingerID id) const
{
    for(uint32_t i = 0; i < MAX_FINGERS; i++) {
        if(_fingers[i] == id)
            return i;
    }
    return MAX_FINGERS;
}

int Backend::addFingerNumber(SDL_FingerID id)
{
    for(uint32_t i = 0; i < MAX_FINGERS; i++) {
        if(_fingers[i] == -1) {
            _fingers[i] = id;
            return i;
        }
    }
    return MAX_FINGERS;
}

