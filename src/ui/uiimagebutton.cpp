#include "uiimagebutton.h"

UIImageButton::UIImageButton(UIManager* manager, const Texture* idle, const Texture* pressed)
 : UIElement(manager, false),
   fingerNumber(-1),
   _idleTexture(idle),
   _pressedTexture(pressed)
{
}

Rect<int> UIImageButton::minBounds() const
{
    return Rect<int>(0, 0, _idleTexture->width(), _idleTexture->height());
}

void UIImageButton::render(Texture* out)
{
    auto texture = fingerNumber >= 0 ? _pressedTexture : _idleTexture;
    
    texture->draw(out, bounds.x1, bounds.y1, 0); 
}

void UIImageButton::update(float dt)
{
    (void) dt;
}

void UIImageButton::onFingerMove(int id, int x, int y, int px, int py)
{
    if(fingerNumber != id)
        return;
    
    if(bounds.inside(px,py) && not bounds.inside(x,y)) {
        fingerNumber = -1;
    }
}

void UIImageButton::onFingerPressed(int id, int x, int y)
{
    (void) x;
    (void) y;
    
    fingerNumber = id;
}

void UIImageButton::onFingerReleased(int id, int x, int y)
{
    (void) x;
    (void) y;
    
    if(fingerNumber != id)
        return;
    
    fingerNumber = -1;
    
    for (auto f : _clickHandlers)
        f();
}
