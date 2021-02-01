#ifndef UIIMAGEBUTTON_H
#define UIIMAGEBUTTON_H

#include <functional>

#include "uielement.h"

#include "util/math.h"

class UIImageButton : public UIElement
{
public:
    UIImageButton(UIManager* manager, const Texture* idle, const Texture* pressed);
  
    Rect<int> minBounds() const override;
    void render(Texture * out) override;
    void update(float dt) override;
    
    void onFingerMove(int id, int x, int y, int px, int py) override;
    void onFingerPressed(int id, int x, int y) override;
    void onFingerReleased(int id, int x, int y) override;
   
	void addClickHandler(const std::function<void()>& func) {
		_clickHandlers.push_back(func);
	}
	
    int fingerNumber;
private:
	std::vector<std::function<void()> > _clickHandlers;
    const Texture* _idleTexture;
    const Texture* _pressedTexture;
};

#endif // UIIMAGEBUTTON_H 
