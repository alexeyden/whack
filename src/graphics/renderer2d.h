#ifndef RENDERER2D_H
#define RENDERER2D_H

#include "util/rect.h"
#include "ninepatch.h"
#include "texture.h"

class Renderer2D
{
public:
	Renderer2D(Texture* canvas);
	
	void drawRect(const Rect<int>& rect);
	void drawRectFilled(const Rect<int>& rect);
	void drawLine(int x1, int y1, int x2, int y2);
    void drawCircle(int x0, int y0, int r);
	
	void drawRectTiled(const Rect<int>& rect, const Texture& tex);
	void drawRectScaledTiled(const Rect<int>& rect, const Texture& tex, float scale);
	void drawNinePatch(const Rect<int>& rect, const NinePatch& np, bool fill);
	
	unsigned penColor;
	unsigned brushColor;	
	unsigned tintColor;
	
protected:
	Texture* _canvas;
};

#endif // RENDERER2D_H
