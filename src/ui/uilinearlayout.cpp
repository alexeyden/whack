#include "uilinearlayout.h"

UILinearLayout::UILinearLayout(UIManager* manager) :
	UILayout(manager, false), spacing(0),
	vertical(true), equalSpace(false), lastFill(false)
{

}

void UILinearLayout::arrange()
{
	if(vertical)
		arrangeVertical();
	else
		arrangeHorizontal();
}

void UILinearLayout::arrangeHorizontal()
{

}

void UILinearLayout::arrangeVertical()
{
	int y = bounds.y1 + paddingY;
	
	int equalDelta = float(bounds.height() - 2 * paddingY - (children().size()-1) * spacing)/children().size();
	
	for(auto it = _children.begin(); it != _children.end(); it++) {
		UIElement* child = *it;
		
		if(lastFill && !equalDelta && it == _children.end()-1) {
			child->bounds.x1 = bounds.x1 + paddingX + child->marginX;
			child->bounds.y1 = y + child->marginY;
			child->bounds.x2 = bounds.x2 - paddingX - child->marginX;
			child->bounds.y2 = bounds.y2 - paddingY - child->marginY;
		}
		
		const Rect<int>& mb = child->minBounds();
		child->bounds.x1 = bounds.x1 + paddingX + child->marginX;
		child->bounds.y1 = y + child->marginY;
		child->bounds.x2 = bounds.x2 - paddingX - child->marginX;
		child->bounds.y2 = child->bounds.y1 + ((equalSpace) ? equalDelta : mb.height()) - child->marginY;
		
		y += (equalSpace) ? equalDelta : mb.height(); 
		
		y += spacing + child->marginY * 2;
	}
}

Rect<int> UILinearLayout::minBounds() const
{
	int childrenSize = 0;
	int maxSize = 0;
	for(UIElement* child : _children) {
		childrenSize += ((vertical) ? child->minBounds().height() : child->minBounds().width()) + spacing;
		maxSize = std::max((vertical) ? child->minBounds().width() : child->minBounds().height(), maxSize);
	}
	childrenSize -= spacing;
	childrenSize += 2 * (vertical ? paddingY : paddingX);
	maxSize += 2 * (vertical ? paddingX : paddingY);
	
	if(vertical)
		return Rect<int>(0, 0, maxSize, childrenSize);
	else
		return Rect<int>(0, 0, childrenSize, maxSize);
}
