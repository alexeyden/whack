#include "uiconsole.h"

#include <algorithm>

#include "controller/inputmanager.h"
#include "graphics/renderer2d.h"

#include "resources/resources.h"

UIConsole::UIConsole(UIManager* manager): UIElement(manager, true),
	_backgroundTex(Resources::i()->atlas("tiles2")->textureByName("bricks_green"))
{
	_edit = new UIEditBox(manager);
	_output = new UILabel(manager, "");
	_output->centered = false;
}

UIConsole::~UIConsole()
{
	delete _edit;
	delete _output;
}

void UIConsole::update(float dt)
{
	_edit->update(dt);
	_output->update(dt);
}

void UIConsole::render(Texture* out)
{
	Renderer2D painter(out);
	painter.drawRectTiled(this->bounds, _backgroundTex);
	
	_edit->render(out);
	_output->render(out);
}

void UIConsole::arrange()
{
	int height = _edit->minBounds().height();
	
	_edit->bounds = bounds;
	_edit->bounds.x1 += 4;
	_edit->bounds.y1 = bounds.y2 - height;
	
	_output->bounds = bounds;
	_output->bounds.x1 += 4;
	_output->bounds.y1 += 2;
	_output->bounds.y2 = bounds.y2 - height - 1;
}

Rect< int > UIConsole::minBounds() const
{
	Rect<int> re = _edit->minBounds();
	Rect<int> ro = _output->minBounds();
	
	return Rect<int>(0, 0, std::max(re.x2, ro.x2), re.y2 + ro.y2);
}

bool UIConsole::onCharEntered(unsigned int ch)
{
	return _edit->onCharEntered(ch);
}

bool UIConsole::onKeyReleased(int key, int mods)
{
	return _edit->onKeyReleased(key, mods);
}

bool UIConsole::onKeyPressed(int key, int mods)
{
	bool r = _edit->onKeyPressed(key, mods);
	
	if(key == InputManager::K_ENTER) {
		std::string result = "";
		_output->text += "\n:" + result;
		_edit->text("");
		
		if(std::count(_output->text.begin(), _output->text.end(), '\n') > 10) {
			_output->text.erase(_output->text.begin(), std::find(_output->text.begin(), _output->text.end(), '\n')+2);
		}
		
		return true;
	}
	
	return r;
}


