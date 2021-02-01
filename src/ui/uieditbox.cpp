#include "uieditbox.h"

#include "controller/inputmanager.h"
#include "graphics/renderer2d.h"

UIEditBox::UIEditBox(UIManager* manager):
	UIElement(manager, true), _pointer(0), _blinkTime(0.0f), _cursor(true)
{

}

void UIEditBox::update(float dt)
{
	_blinkTime += dt;
	
	const float blinkPeriod = 0.2f;
	if(_blinkTime > blinkPeriod) {
		_blinkTime -= blinkPeriod;
		_cursor = !_cursor;
	}
}

void UIEditBox::render(Texture* out)
{
	_manager->style()->fontNormal.drawText(
		out,
		bounds.x1, bounds.y1,
		_text.c_str()
	);
	
	if(_cursor) {
		char caret[] = "_";
		_manager->style()->fontNormal.drawText(
			out,
			bounds.x1 + _manager->style()->fontNormal.charSize() * _pointer,
			bounds.y1,
			caret
		);
	}
}

Rect< int > UIEditBox::minBounds() const
{
	int charSize = _manager->style()->fontNormal.charSize();
	int width = _text.size() * charSize + paddingX * 2;
	int height = charSize + paddingY * 2;
	
	return Rect<int>(0, 0, width, height);
}

void UIEditBox::text(const std::string newtext)
{
	_text = newtext;
	_pointer = 0;
}

bool UIEditBox::onCharEntered(unsigned int ch)
{
	_text.insert(_pointer, 1, (unsigned char) ch);
	_pointer++;
	
	return true;
}

bool UIEditBox::onKeyReleased(int key, int mods)
{
    (void) mods;
    
	if(key == InputManager::K_BACKSPACE) {
		if(_pointer > 0) {
			_text.erase(_pointer - 1, 1);
			_pointer--;
		}
		
		return true;
	}
	
	return false;
}

