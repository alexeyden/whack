#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <vector>

#include "util/rect.h"
#include "controller/controller.h"
#include "graphics/texture.h"

#include "uimanager.h"

class UIElement : public Controller {
public:
	UIElement(UIManager* manager, bool allowFocus) :
		paddingX(0), paddingY(0), marginX(0), marginY(0), disabled(false),
		_manager(manager), _allowFocus(allowFocus),
		_pressed(false) {}
    virtual ~UIElement() {}
	
	virtual Rect<int> minBounds() const = 0;
	
	virtual void render(Texture* out) = 0; 
	virtual void update(float dt) = 0;
	
	Rect<int> bounds;

	int paddingX, paddingY;
	int marginX, marginY;
	
	void setMargin(int margin) {
		marginX = margin; marginY = margin;
	}
	
	void setPadding(int padding) {
		paddingX = padding; paddingY = padding;
	}
	
	UIElement* parent() {
		return _parent;
	}
	
	UIManager* manager() {
		return _manager;
	}
	
	virtual bool onMousePressed(double x, double y, int button, int mods) override {
        (void) x;
        (void) y;
        (void) button;
        (void) mods;
        
		if(disabled)
			return false;
		
		if(_allowFocus)
			_manager->setFocus(this);
		_pressed = true;
		
		return false;
	}
	
	virtual bool onMouseReleased(double x, double y, int button, int mods) override {
        (void) x;
        (void) y;
        (void) button;
        (void) mods;
        
		if(disabled)
			return false;
		
		_pressed = false;
		
		return false;
	}
	
	virtual void onMouseExit(double x, double y, int button, int mods) {
        (void) x;
        (void) y;
        (void) button;
        (void) mods;
        
        _pressed = false;
    }
	
	virtual bool onCharEntered(unsigned int) override { return false; }
	virtual bool onKeyPressed(int, int) override { return false; }
	virtual bool onKeyReleased(int, int) override { return false; }
	virtual void onMouseMove(double, double, double, double, int) override {}
	virtual void onFingerMove(int, int, int, int, int) override {}
	virtual void onFingerPressed(int, int, int) override {}
    virtual void onFingerReleased(int, int, int) override {}
    virtual void onFingerExit(int id, int x, int y) {
        (void) id;
        (void) x;
        (void) y;
    }
	
	bool disabled;
	bool invisible;
protected:
	bool isPressed() const {
		return _pressed;
	}
	
	UIElement* _parent;
	UIManager* _manager;
	bool _allowFocus;
	
private:
	bool _pressed;
};

#endif
