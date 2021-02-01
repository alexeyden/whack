#ifndef UIIMAGE_H
#define UIIMAGE_H

#include "uielement.h"
#include "graphics/texture.h"

class UIImage : public UIElement
{
public:
	UIImage(UIManager* manager, const Texture* texture);
	
	virtual void update(float dt);
	virtual void render(Texture* out);
	
	virtual Rect< int > minBounds() const;
	
	unsigned tintColor;
private:
	const Texture* _image;
};

#endif // UIIMAGE_H
