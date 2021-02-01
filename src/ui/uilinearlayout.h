#ifndef UILINEARLAYOUT_H
#define UILINEARLAYOUT_H

#include "uilayout.h"

#include "graphics/renderer2d.h"

class UIManager;

class UILinearLayout : public UILayout
{
public:
	UILinearLayout(UIManager* manager);
	
	virtual void arrange() override;
	virtual Rect<int> minBounds() const override;

	unsigned spacing;
	
	bool vertical;
	bool equalSpace;
	bool lastFill;
private:
	void arrangeVertical();
	void arrangeHorizontal();
};

#endif // UILINEARLAYOUT_H
