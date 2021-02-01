#ifndef UILABEL_H
#define UILABEL_H

#include <string> 

#include "uielement.h"
#include "uimanager.h"

class UILabel : public UIElement
{
public:
	UILabel(UIManager* manager, std::string str);
	
	virtual void update(float dt) {
        (void) dt;
    }
	virtual void render(Texture* out);
	virtual Rect<int> minBounds() const;
	
	bool centered;
	
	std::string text;
};

#endif // UILABEL_H
