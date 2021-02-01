#ifndef UIJOYPAD_H
#define UIJOYPAD_H

#include "uielement.h"

#include "util/math.h"

class UIJoyPad : public UIElement
{
public:
    UIJoyPad(UIManager* manager, const Texture* back, const Texture* frontN, const Texture* frontP);
  
    Rect<int> minBounds() const override;
    void render(Texture * out) override;
    void update(float dt) override;
    
    void onFingerExit(int id, int x, int y) override;
    void onFingerMove(int id, int x, int y, int px, int py) override;
    void onFingerPressed(int id, int x, int y) override;
    void onFingerReleased(int id, int x, int y) override;
   
    vec2 factor() const;
    
    int fingerNumber;
private:
    static const int MAX_DSPL;
    int dx, dy;
    
    const Texture* backTexture;
    const Texture* frontNormalTexture;
    const Texture* frontPressedTexture;
};

#endif // UIJOYPAD_H
