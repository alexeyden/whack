#ifndef UILAYOUT_H
#define UILAYOUT_H

#include <vector>

#include "uielement.h"
#include "graphics/texture.h"

class UILayout : public UIElement {
public:
  UILayout(UIManager* manager, bool allowFocus) :
		UIElement(manager, allowFocus) {}
	
	virtual void render(Texture* out) override; 
	virtual void update(float dt) override;
	
	virtual void arrange() = 0;
	
	const std::vector<UIElement*>& children() const {
		return _children;
	}
	
	void addChild(UIElement* el) {
		_children.push_back(el);
	}
	
	virtual bool onCharEntered(unsigned int ch) override;
	virtual bool onKeyPressed(int key, int mods) override;
	virtual bool onKeyReleased(int key, int mods) override;
	virtual void onMouseMove(double prevX, double prevY, double x, double y, int buttons) override;
	virtual bool onMousePressed(double x, double y, int button, int mods) override;
	virtual bool onMouseReleased(double x, double y, int button, int mods) override;
	
protected:
	std::vector<UIElement*> _children;
};

#endif // UILAYOUT_H
