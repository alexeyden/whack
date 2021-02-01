#ifndef UIEDITBOX_H
#define UIEDITBOX_H

#include <string>

#include "uielement.h"

class UIEditBox : public UIElement
{
public:
	UIEditBox(UIManager* manager);
	
	virtual void update(float dt);
	virtual void render(Texture* out);
	
	virtual Rect< int > minBounds() const;
	
	virtual bool onCharEntered(unsigned int ch);
	virtual bool onKeyReleased(int key, int mods);

	void text(const std::string newtext);
	const std::string& text() const { return _text; }
private:	
	std::string _text;

	size_t _pointer;
	float _blinkTime;
	bool _cursor;
};

#endif // UIEDITBOX_H
