#include "uicheckbox.h"

UICheckBox::UICheckBox(UIManager* manager): UIElement(manager, false), checked(false)
{
}

void UICheckBox::render(Texture* out)
{
	unsigned color = isPressed() ? _manager->style()->colorActivated : _manager->style()->colorNormal;
	
	if(checked) 
		_manager->style()->checkboxChecked.draw(out, bounds.x1 + paddingX, bounds.y1 + paddingY, color);
	else
		_manager->style()->checkboxUnchecked.draw(out, bounds.x1 + paddingX, bounds.y1 + paddingY, color);

	const Font& font = isPressed() ? _manager->style()->fontActivated : _manager->style()->fontNormal;
	const Rect<int>& min = minBounds();
	
	font.drawText(out, bounds.x1 + paddingX  + _manager->style()->checkboxChecked.width() + gap,
								bounds.y1 + min.centerY() - font.charSize()/2, text.c_str());
}

void UICheckBox::update(float dt)
{
    (void) dt;
}

Rect< int > UICheckBox::minBounds() const
{
	return Rect<int>(
		0, 0,
		_manager->style()->checkboxChecked.width() + gap + _manager->style()->fontNormal.textBounds(text).first + paddingX * 2,
		std::max(_manager->style()->fontNormal.charSize(), _manager->style()->checkboxChecked.height()) + paddingX * 2
	);
}

bool UICheckBox::onMouseReleased(double x, double y, int button, int mods)
{
	UIElement::onMouseReleased(x, y, button, mods);

	checked = !checked;
	
	for(auto& ch : _clickHandlers)
		ch(checked);
	
	return true;
}
