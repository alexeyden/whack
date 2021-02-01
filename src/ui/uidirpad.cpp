#include "uidirpad.h"

#include "SDL_log.h"

#include "util/math.h"

UIDirPad::UIDirPad(UIManager* manager, const Texture* normal[4], const Texture* pressed[4]) :
    UIElement(manager, false), _state(0), fingerSize(10)
{
    for (uint32_t i = 0; i < 4; i++) {
        _normal[i] = normal[i];
        _pressed[i] = pressed[i];
    }
}

Rect<int> UIDirPad::minBounds() const
{
    int width = _normal[0]->width() * 2 - 1;
    int height = _normal[3]->height() * 2 - 1;
    
    return Rect<int>(0, 0, width, height);
}

void UIDirPad::render(Texture* out)
{
    // left
    {
        const Texture* image = _state & 0b0001 ? _pressed[0] : _normal[0];
        image->draw(out, bounds.x1, bounds.centerY() - image->height()/2, 0x00);
    }
   
    // right
    {
        const Texture* image = _state & 0b0010 ? _pressed[1] : _normal[1];
        image->draw(out, bounds.x1 + image->width() - 1, bounds.centerY() - image->height()/2, 0x00);
    }
    
    // top
    {
        const Texture* image = _state & 0b0100 ? _pressed[2] : _normal[2];
        image->draw(out, bounds.x1 + bounds.centerX() - image->width() - 2, bounds.y1, 0x00);
    }
    
    // down 
    {
        const Texture* image = _state & 0b1000 ? _pressed[3] : _normal[3];
        image->draw(out, bounds.x1 + bounds.centerX() - image->width() - 2, bounds.y1 + image->height() - 1, 0x00);
    }
}

void UIDirPad::update(float dt)
{
    (void) dt;
}

void UIDirPad::updateState(vec2 c0)
{ 
    float cr = fingerSize;
    
    vec2 vtl(bounds.x1, bounds.y1);
    vec2 vtr(bounds.x2, bounds.y1);
    vec2 vbr(bounds.x2, bounds.y2);
    vec2 vbl(bounds.x1, bounds.y2);
    vec2 vc(bounds.centerX(), bounds.centerY());
    
    if(circleTriangleIntersect(c0, cr, vc, vtl, vtr)) {
        _state |= 0b0100; // top
    }
    if(circleTriangleIntersect(c0, cr, vbl, vc, vbr)) {
        _state |= 0b1000; // bottom
    }
    if(circleTriangleIntersect(c0, cr, vbr, vc, vtr)) {
        _state |= 0b0010; // right
    }
    if(circleTriangleIntersect(c0, cr, vbl, vtl, vc)) {
        _state |= 0b0001; // left
    }
}

void UIDirPad::onFingerPressed(int num, int x, int y)
{
    if(fingerNum >= 0 && fingerNum != num)
        return;
    
    fingerNum = num;
    
    updateState(vec2(x,y));
}

void UIDirPad::onFingerReleased(int num, int x, int y)
{
    if(fingerNum != num)
        return;
   
    fingerNum = -1;
    _state = 0;
}

void UIDirPad::onFingerMove(int num, int x, int y, int px, int py)
{
    (void) px;
    (void) py;
    
    if(fingerNum != num)
        return;
    
    updateState(vec2(x,y));
}
