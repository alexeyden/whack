#ifndef UIMANAGER_CPP
#define UIMANAGER_CPP

#include "uimanager.h"
#include "uielement.h"
#include "uistyle.h"

UIManager::UIManager(UIStyle* style)
: _focus(nullptr), _style(style)
{
}

bool UIManager::onCharEntered(unsigned int ch)
{
	if(_focus != nullptr)
		return _focus->onCharEntered(ch);
	
	return false;
}

bool UIManager::onKeyPressed(int key, int mods)
{
	if(_focus != nullptr)
		return _focus->onKeyPressed(key, mods);
	
	return false;
}

bool UIManager::onKeyReleased(int key, int mods)
{
	if(_focus != nullptr) {
		return _focus->onKeyReleased(key, mods);
	}
	return false;
}

void UIManager::onMouseMove(double px, double py, double x, double y, int buttons)
{
	if(root != nullptr && root->bounds.inside(x, y))
		root->onMouseMove(
            px - root->bounds.x1,
            py - root->bounds.y1,
            x - root->bounds.x1,
            y - root->bounds.y1,
            buttons);
}

bool UIManager::onMousePressed(double x, double y, int button, int mods)
{
	int vpX = x;
	int vpY = y;
	
	if(root != nullptr && root->bounds.inside(vpX,vpY))
		return root->onMousePressed(vpX - root->bounds.x1, vpY - root->bounds.y1, button, mods);
	
	return false;
}

bool UIManager::onMouseReleased(double x, double y, int button, int mods)
{
	int vpX = x;
	int vpY = y;
	
	if(root != nullptr && root->bounds.inside(vpX,vpY))
		return root->onMouseReleased(vpX - root->bounds.x1, vpY - root->bounds.y1, button, mods);
	
	return false;
}

#endif // UIMANAGER_CPP
