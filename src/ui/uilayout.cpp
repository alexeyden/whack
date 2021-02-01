#include "uilayout.h"

#include "SDL_log.h"

void UILayout::render(Texture* out)
{
	for(UIElement* child : _children) {
		child->render(out);
	}
}

void UILayout::update(float dt)
{
	for(UIElement* child : _children) {
		child->update(dt);
	}
}

void UILayout::onMouseMove(double prevX, double prevY, double x, double y, int buttons)
{
    UIElement* prev = nullptr;
    UIElement* hover = nullptr;
    
	for(UIElement* child : _children) {
        if(child->bounds.inside(bounds.x1 + prevX, bounds.y1 + prevY))
            prev = child;
        
		if(child->bounds.inside(bounds.x1 + x, bounds.y1 + y)) {
			hover = child;
            break;
		}
	}
	
	if(hover != nullptr)
        hover->onMouseMove(prevX + bounds.x1 - hover->bounds.x1, prevY + bounds.y1 - hover->bounds.y1,
                           x + bounds.x1 - hover->bounds.x1, y + bounds.y1 - hover->bounds.y1, buttons);
        
	if(prev != hover && prev != nullptr) {
        prev->onMouseExit(prevX + bounds.x1 - prev->bounds.x1, prevY + bounds.y1 - prev->bounds.y1, buttons, 0);
    }
}

bool UILayout::onMousePressed(double x, double y, int button, int mods)
{
	for(UIElement* child : _children) {
		if(child->bounds.inside(bounds.x1 + x, bounds.y1 + y)) {
			child->onMousePressed(x + bounds.x1 - child->bounds.x1, y + bounds.y1 - child->bounds.y1, button, mods);
		}
	}
	
	return true;
}

bool UILayout::onMouseReleased(double x, double y, int button, int mods)
{
	for(UIElement* child : _children) {
		if(child->bounds.inside(x,y)) {
			child->onMouseReleased(x + bounds.x1 - child->bounds.x1, y + bounds.y1 - child->bounds.y1, button, mods);
		}
	}
	
	return true;
}

bool UILayout::onCharEntered(unsigned int ch)
{
    (void) ch;
	return false;
}

bool UILayout::onKeyPressed(int key, int mods)
{
    (void) key;
    (void) mods;
    
	return false;
}

bool UILayout::onKeyReleased(int key, int mods)
{
    (void) key;
    (void) mods;
    
	return false;
}
