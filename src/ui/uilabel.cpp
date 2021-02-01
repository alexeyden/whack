#include "uilabel.h"

#include "uistyle.h"
#include "graphics/renderer2d.h"

UILabel::UILabel(UIManager* manager, std::string str)
	: UIElement(manager, false), centered(true), text(str)
{

}

void UILabel::render(Texture* out)
{
	Rect<int> text_bounds = minBounds();
	if(centered) {
		_manager->style()->fontNormal.drawText(
			out,
			bounds.centerX() - text_bounds.width()/2, bounds.centerY() - text_bounds.height()/2,
			text.c_str()
		);
	}
	else {
		_manager->style()->fontNormal.drawText(
			out,
			bounds.x1, bounds.y1,
			text.c_str(),
			bounds.width(), bounds.height()
		);
	}
}

Rect<int> UILabel::minBounds() const
{
	
	std::pair<int, int> text_size = _manager->style()->fontNormal.textBounds(text);
	
	int width = text_size.first + paddingX * 2;
	int height = text_size.second + paddingY * 2;
	
	return Rect<int>(0, 0, width, height);
}
