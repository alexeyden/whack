#include "uiimage.h"

UIImage::UIImage(UIManager* manager, const Texture* texture):
	UIElement(manager, false),  tintColor(0x00), _image(texture)
{
}

void UIImage::update(float dt)
{
    (void) dt;
}

void UIImage::render(Texture* out)
{
	_image->draw(out,
                 bounds.centerX() - _image->width()/2,
                 bounds.centerY() - _image->height()/2,
                 tintColor);
}

Rect< int > UIImage::minBounds() const
{
	return Rect<int>(0, 0, paddingX + _image->width(), paddingY + _image->height());
}
