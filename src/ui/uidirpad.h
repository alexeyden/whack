#ifndef UIIMAGEBUTTON_H
#define UIIMAGEBUTTON_H

#include <functional>

#include "ui/uielement.h"

class vec2;

class UIDirPad : public UIElement
{
public:
    UIDirPad(UIManager* manager, const Texture* normal[4], const Texture* pressed[4]);
    
	virtual void update(float dt);
	virtual void render(Texture* out);
	
	virtual Rect< int > minBounds() const;
    
    void onFingerPressed(int num, int x, int y) override;
    void onFingerReleased(int num, int x, int y) override;
    void onFingerMove(int num, int x, int y, int px, int py) override;
    
    int fingerSize;
    int fingerNum;
private:
    void updateState(vec2 c0);
    
	std::vector<std::function<void()> > _pressHandlers;
    std::vector<std::function<void()> > _releaseHandlers;
   
    uint32_t _state;
    const Texture* _normal[4];
    const Texture* _pressed[4];
};

#endif // UIIMAGEBUTTON_H
