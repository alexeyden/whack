#ifndef UIBUTTON_H
#define UIBUTTON_H

#include <string>
#include <functional>
#include <vector>

#include "uielement.h"

class UIButton : public UIElement
{
public:
	UIButton(UIManager* manager);
	
	virtual void update(float dt) override;
	virtual void render(Texture* out) override;
	
	virtual Rect< int > minBounds() const override;
	
	std::string text;

	virtual bool onMouseReleased(double x, double y, int button, int mods) override;
    
	void addClickHandler(const std::function<void()>& func) {
		_clickHandlers.push_back(func);
	}
private:
	std::vector<std::function<void()> > _clickHandlers;
};

#endif // UIBUTTON_H
