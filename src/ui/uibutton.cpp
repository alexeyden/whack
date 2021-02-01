#include "uibutton.h"

#include "SDL.h"

#include "graphics/renderer2d.h"
#include "util/util.h"

#include "uistyle.h"

UIButton::UIButton(UIManager* manager) : UIElement(manager, false)
{

}

void UIButton::update(float dt)
{
    (void) dt;
}

void UIButton::render(Texture* out)
{
	Renderer2D r(out);
	
	if(disabled)
		r.tintColor = _manager->style()->colorDisabled;
	else
		r.tintColor = _manager->style()->colorNormal;
	
	if(isPressed())
		r.tintColor = _manager->style()->colorActivated;
	
	const NinePatch& patch = isPressed() ? _manager->style()->buttonPressed : _manager->style()->buttonNormal;
	r.drawNinePatch(bounds, patch, true);
	
	int charSize = _manager->style()->fontNormal.charSize();
	Rect<int> textRect(0, 0, charSize * text.size(), charSize);

	const Font& font = isPressed() ?
		_manager->style()->fontActivated :
		(disabled) ?
			_manager->style()->fontDisabled :
			_manager->style()->fontNormal;
			
	font.drawText(
		out,
		bounds.centerX() - textRect.width()/2, bounds.centerY() - textRect.height()/2 + (isPressed() ? 1 : 0),
		text.c_str()
	);
}

Rect< int > UIButton::minBounds() const
{
	int width = text.size() * _manager->style()->fontNormal.charSize()
		+ _manager->style()->buttonNormal.left.width()
		+ _manager->style()->buttonNormal.right.width()
		+ paddingX * 2;
		
	int height = _manager->style()->fontNormal.charSize()
		+ _manager->style()->buttonNormal.top.height()
		+ _manager->style()->buttonNormal.bottom.height()
		+ paddingY * 2;
	
	return Rect<int>(0, 0, width, height);
}

bool UIButton::onMouseReleased(double x, double y, int button, int mods)
{
	UIElement::onMouseReleased(x, y, button, mods);
	
	if(disabled)
		return false;
	
	for(auto& ch : _clickHandlers)
		ch();
	
	return true;
}


