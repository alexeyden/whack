#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <map>

#include "controller.h"
#include "backend/backend.h"

class InputManager : private Controller
{
public:
	static void init(Backend* backend);
	static void destroy();

	static void addController(Controller* controller);
	static void removeController(Controller* controller);
    
    enum Key {
        FORWARD = 1,
        BACKWARD,
        TURN_LEFT,
        TURN_RIGHT,
        STRAFE_LEFT,
        STRAFE_RIGHT,
        
        JUMP,
        FIRE,
        
        DEBUG_1,
        DEBUG_2,
        
        K_ESCAPE,
        K_BACKSPACE,
        K_ENTER
    };
    
private:
    InputManager() {}
    virtual ~InputManager() {}
    
	bool onMouseReleased(double x, double y, int button, int mods) override;
	bool onMousePressed(double x, double y, int button, int mods) override;
	void onMouseMove(double px, double py, double x, double y, int buttons) override;
	
	bool onKeyPressed(int key, int mods) override;
	bool onKeyReleased(int key, int mods) override;
	bool onCharEntered(unsigned int ch) override;
    
    void onFingerMove(int num, int x, int y, int px, int py) override;
    void onFingerPressed(int num, int x, int y) override;
    void onFingerReleased(int num, int x, int y) override;
	
	static std::vector<Controller*> _controllers;
    static std::map<unsigned, unsigned> _keymap;
    
    static InputManager* _instance;
};

#endif // INPUT_H
