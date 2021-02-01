#ifndef UICONSOLE_H
#define UICONSOLE_H

#include "uielement.h"
#include "uilabel.h"
#include "uieditbox.h"
#include "graphics/renderer2d.h"

class UIConsole : public UIElement
{
public:
	UIConsole(UIManager* manager);
	virtual ~UIConsole();
	
	virtual void update(float dt);
	virtual void render(Texture* out);
	virtual Rect< int > minBounds() const;
	
	void arrange();
	
	virtual bool onCharEntered(unsigned int ch);
	virtual bool onKeyReleased(int key, int mods);
	virtual bool onKeyPressed(int key, int mods);
	
private:
	UIEditBox* _edit;
	UILabel* _output;
	const Texture& _backgroundTex;
};

#endif // UICONSOLE_H
