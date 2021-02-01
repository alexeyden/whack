#ifndef UISETTINGS_H
#define UISETTINGS_H

#include "graphics/font.h"
#include "controller/controller.h"
#include "ui/uistyle.h"

class UIElement;

class UIManager : public Controller {
public:
	UIManager(UIStyle* style); 
    virtual ~UIManager() {}

	const UIStyle* style() const {
		return _style;
	}	
	
	UIElement* focus() {
		return _focus;
	}
	
	void setFocus(UIElement* f) {
		_focus = f;
	}
	
	virtual bool onCharEntered(unsigned int ch) override;
	virtual bool onKeyPressed(int key, int mods) override;
	virtual bool onKeyReleased(int key, int mods) override;
	virtual void onMouseMove(double px, double py, double x, double y, int buttons) override;
	virtual bool onMousePressed(double x, double y, int button, int mods) override;
	virtual bool onMouseReleased(double x, double y, int button, int mods) override;
    
	virtual void onFingerMove(int num, int x, int y, int px, int py) override {
        (void) num;
        (void) x;
        (void) y;
        (void) px;
        (void) py;
    }
	virtual void onFingerPressed(int num, int x, int y) override {
        (void) num;
        (void) x;
        (void) y;
    }
    virtual void onFingerReleased(int num, int x, int y) override {
        (void) num;
        (void) x;
        (void) y;
    }

	UIElement* root;
private:
	UIElement* _focus;
	
	UIStyle* _style;
};

#endif // UISETTINGS_H
