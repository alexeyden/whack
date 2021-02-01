#include "renderer2d.h"

#include <cmath>

Renderer2D::Renderer2D(Texture* canvas) 
	: penColor(0x00000000), brushColor(0x00000000), tintColor(0x00000000), _canvas(canvas)
{

}

void Renderer2D::drawCircle(int x0, int y0, int r)
{
    int x = r - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1);
    
    while( x >= 0) {
        _canvas->setPixel(x0 + x, y0 + y, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
        _canvas->setPixel(x0 + y, y0 + x, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
        _canvas->setPixel(x0 - y, y0 + x, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
        _canvas->setPixel(x0 - x, y0 + y, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
        _canvas->setPixel(x0 - x, y0 - y, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
        _canvas->setPixel(x0 - y, y0 - x, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
        _canvas->setPixel(x0 + y, y0 - x, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
        _canvas->setPixel(x0 + x, y0 - y, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-r << 1) + dx;
        }
    }
}

void Renderer2D::drawRect(const Rect< int >& rect)
{
	for(int x = rect.x1; x <= rect.x2; x++) {
		_canvas->setPixel(x, rect.y1, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
		_canvas->setPixel(x, rect.y2, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
	}
	for(int y = rect.y1; y <= rect.y2; y++) {
		_canvas->setPixel(rect.x1, y, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
		_canvas->setPixel(rect.x2, y, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
	}
}

void Renderer2D::drawRectFilled(const Rect< int >& rect)
{
	for(int x = rect.x1; x <= rect.x2; x++) {
		for(int y = rect.y1; y <= rect.y2; y++) {
			if(x == rect.x1 || x == rect.x2 || y == rect.y1 || y == rect.y2)
				_canvas->setPixel(x, y, penColor & 0xff, (penColor & 0x0000ff00) >> 8, (penColor & 0x00ff0000) >> 16);
			else
				_canvas->setPixel(x, y, brushColor & 0xff, (brushColor & 0x0000ff00) >> 8, (brushColor & 0x00ff0000) >> 16);
		}
	}
}

void Renderer2D::drawLine(int x1, int y1, int x2, int y2)
{
	unsigned b = penColor & 0xff;
	unsigned g = (penColor & 0x0000ff00) >> 8;
	unsigned r = (penColor & 0x00ff0000) >> 16;
	
	const bool steep = fabs(y2 - y1) > fabs(x2 - x1);
	
	if(steep) {
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	
	if(x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}
	
	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);
	
	float err = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = y1;
	const int xmax = x2;
	
	for(int x = x1; x < xmax; x++) {
		if(steep)
			_canvas->setPixel(y,x, r, g, b);
		else
			_canvas->setPixel(x,y, r, g, b);
		err -= dy;
		if(err < 0) {
			y += ystep;
			err += dx;
		}
	}
}

void Renderer2D::drawNinePatch(const Rect<int>& rect, const NinePatch& np, bool fill)
{
	np.topLeft.draw(_canvas, rect.x1, rect.y1, tintColor);
	np.topRight.draw(_canvas, rect.x2 - np.topRight.width() + 1, rect.y1, tintColor);
	np.bottomLeft.draw(_canvas, rect.x1, rect.y2 - np.bottomLeft.height() + 1, tintColor);
	np.bottomRight.draw(_canvas, rect.x2 - np.bottomRight.width() + 1, rect.y2 - np.bottomRight.height() + 1, tintColor);
	
	Rect<int> r;
	r.x1 = rect.x1 + np.topLeft.width(); r.y1 = rect.y1;
	r.x2 = rect.x2 - np.topRight.width() + 1; r.y2 = rect.y1 + np.top.height();
	drawRectTiled(r, np.top);
	
	r.x1 = rect.x1 + np.bottomLeft.width(); r.y1 = rect.y2 - np.bottom.height() + 1;
	r.x2 = rect.x2 - np.bottomRight.width() + 1; r.y2 = rect.y2;
	drawRectTiled(r, np.bottom);
	
	r.x1 = rect.x1; r.y1 = rect.y1 + np.topLeft.height();
	r.x2 = rect.x1 + np.left.width(); r.y2 = rect.y2 - np.bottomLeft.height();
	drawRectTiled(r, np.left);
	
	r.x1 = rect.x2 - np.right.width() + 1; r.y1 = rect.y1 + np.topRight.height();
	r.x2 = rect.x2; r.y2 = rect.y2 - np.bottomRight.height();
	drawRectTiled(r, np.right);
	
	if(fill) {
		r.x1 = rect.x1 + np.topLeft.width(); r.y1 = rect.y1 + np.topLeft.height();
		r.x2 = rect.x2 - np.bottomRight.width(); r.y2 = rect.y2 - np.bottomRight.height();
		drawRectTiled(r, np.center);
	}
}

void Renderer2D::drawRectScaledTiled(const Rect< int >& rect, const Texture& tex, float scale)
{
	int xtiles = rect.width()/(scale * tex.width()) + 1, ytiles = rect.height()/(scale * tex.height()) + 1;
	
	for(int xi = 0; xi < xtiles; xi++) {
		for(int yi = 0; yi < ytiles; yi++) {
			int x = xi * tex.width() * scale + rect.x1;
			int y = yi * tex.height() * scale + rect.y1;
			int w = (xi == xtiles - 1) ? rect.x2 - x + 1 : tex.width() * scale; 
			int h = (yi == ytiles - 1) ? rect.y2 - y + 1 : tex.height() * scale;
			int sW = (xi == xtiles - 1) ? (rect.x2 - x + 1)/scale : tex.width();
			int sH = (yi == ytiles - 1) ? (rect.y2 - y + 1)/scale : tex.height();
			
			tex.drawScaled(_canvas, x, y, w, h, 0, 0, sW, sH, tintColor); 
		}
	}
}

void Renderer2D::drawRectTiled(const Rect<int>& rect, const Texture& tex)
{
	int xtiles = rect.width()/tex.width() + 1, ytiles = rect.height()/tex.height() + 1;
	
	for(int xi = 0; xi < xtiles; xi++) {
		for(int yi = 0; yi < ytiles; yi++) {
			int x = xi * tex.width() + rect.x1;
			int y = yi * tex.height() + rect.y1;
			int w = (xi == xtiles - 1) ? rect.x2 - x + 1 : tex.width(); 
			int h = (yi == ytiles - 1) ? rect.y2 - y + 1 : tex.height();
			
			tex.draw(_canvas, x, y, 0, 0, w, h, tintColor); 
		}
	}
}
