#include "game/application.h"
#include "game/menuscreen.h"

#include "SDL_main.h"
#include "SDL_log.h"

int main( int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    
	Application::init(320, 240);
	
	Application::I()->setScreen(new MenuScreen());
	Application::I()->run();
	Application::I()->destroy();
    
    exit(0);

	return 0;
}
