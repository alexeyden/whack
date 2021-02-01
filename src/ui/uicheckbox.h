#ifndef UICHECKBOX_H
#define UICHECKBOX_H

#include <functional>

#include "ui/uielement.h"

class UICheckBox : public UIElement
{
	public:
	UICheckBox(UIManager* manager);
	
	virtual void update(float dt);
	virtual void render(Texture* out);
	
	virtual Rect< int > minBounds() const;
	
	std::string text;
	bool checked;

	virtual bool onMouseReleased(double x, double y, int button, int mods) override;
	
	void addClickHandler(const std::function<void(bool)>& func) {
		_clickHandlers.push_back(func);
	}
	
	const unsigned gap = 10;
private:
	std::vector<std::function<void(bool)> > _clickHandlers;
};

#endif // UICHECKBOX_H
