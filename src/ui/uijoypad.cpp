#include "uijoypad.h"

const int UIJoyPad::MAX_DSPL = 20;

UIJoyPad::UIJoyPad(UIManager* manager, const Texture* back, const Texture* frontN, const Texture* frontP)
 : UIElement(manager, false),
   fingerNumber(-1),
   dx(0), dy(0),
   backTexture(back),
   frontNormalTexture(frontN),
   frontPressedTexture(frontP)
{
    dx = 0;
    dy = 0;
}

Rect<int> UIJoyPad::minBounds() const
{
    return Rect<int>(0, 0, backTexture->width(), backTexture->height());
}

void UIJoyPad::render(Texture* out)
{
    auto stick = fingerNumber >= 0 ? frontPressedTexture : frontNormalTexture;
    
    backTexture->draw(out, bounds.x1, bounds.y1, 0);
    stick->draw(out, bounds.centerX() - stick->width()/2 + dx, bounds.centerY() - stick->height()/2 + dy, 0); 
}

void UIJoyPad::update(float dt)
{
    (void) dt;
}

void UIJoyPad::onFingerExit(int id, int x, int y)
{
    (void) x;
    (void) y;
    
    if(fingerNumber != id)
        return;
    
    dx = 0;
    dy = 0;
    fingerNumber = -1;
}

void UIJoyPad::onFingerMove(int id, int x, int y, int px, int py)
{
    (void) px;
    (void) py;
    
    if(fingerNumber != id)
        return;
    
    dx = std::min(std::max(x - bounds.centerX(), -MAX_DSPL), MAX_DSPL);
    dy = std::min(std::max(y - bounds.centerY(), -MAX_DSPL), MAX_DSPL);
}

void UIJoyPad::onFingerPressed(int id, int x, int y)
{
    (void) x;
    (void) y;
    
    fingerNumber = id;
}

void UIJoyPad::onFingerReleased(int id, int x, int y)
{
    (void) x;
    (void) y;
    
    if(fingerNumber != id)
        return;
    
    dx = 0;
    dy = 0;
    fingerNumber = -1;
}

vec2 UIJoyPad::factor() const
{
    return vec2(dx / float(MAX_DSPL), dy / float(MAX_DSPL));
}
