#include "texture.h"
#include <cmath>
#include <cstring>
#include <algorithm>


#include "SDL_log.h"


Texture::Texture(uint8_t* data, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned dataW, unsigned dataH) :
	_x(x), _y(y), _width(w), _height(h), _dataWidth(dataW), _dataHeight(dataH), _data(data) {}

Texture::Texture() :
	_x(0), _y(0), _width(0), _height(0), _dataWidth(0), _dataHeight(0), _data(nullptr) {}

void Texture::draw(Texture* dst, int dstX, int dstY, int srcX, int srcY, int srcW, int srcH, unsigned int tintColor) const
{
	for(int x = 0; x < srcW; x++) {
		if(x + dstX >= (int) dst->_width) break;
	
		for(int y = 0; y < srcH; y++) {
			if(y + dstY >= (int) dst->_height) break;
			if(pixel(srcX + x, srcY + y) == keyColor)
				continue;
			
			const unsigned char* bgr = pixelPtr(srcX + x, srcY + y);
			if(tintColor != 0) {
				unsigned char r = float(bgr[2] * (tintColor & 0xff))/255.0f;
				unsigned char g = float(bgr[1] * ((tintColor >> 8) & 0xff))/255.0f;
				unsigned char b = float(bgr[0] * (tintColor  >> 16))/255.0f;
				dst->setPixelFast(dstX + x, dstY + y, r, g, b);
			} else {
				dst->setPixelFast(dstX + x, dstY + y, bgr[2], bgr[1], bgr[0]);
			}
		}
	}
}

void Texture::drawScaled(Texture* dst,
												 int dstX, int dstY, int dstW, int dstH,
												 int srcX, int srcY, int srcW, int srcH,
												 unsigned int tintColor) const
{
	for(int x = dstX; x < dstX + dstW; x++) {
		for(int y = dstY; y < dstY + dstH; y++) {
			
			int sX = srcX + ((x - dstX) * srcW)/dstW;
			int sY = srcY + ((y - dstY) * srcH)/dstH;
			
			const unsigned char* bgr = pixelPtr(sX, sY);
			
			if(bgr[2] == keyColorR && bgr[1] == keyColorG && bgr[0] == keyColorB)
				continue;
			
			if(tintColor != 0) {
				unsigned char r = float(bgr[2] * (tintColor & 0xff))/255.0f;
				unsigned char g = float(bgr[1] * ((tintColor >> 8) & 0xff))/255.0f;
				unsigned char b = float(bgr[0] * (tintColor  >> 16))/255.0f;
				dst->setPixelFast(x, y, r, g, b);
			} else {
				dst->setPixelFast(x, y, bgr[2], bgr[1], bgr[0]);
			}	
		}
	}
}

void Texture::draw(Texture* dst, int dstX, int dstY, unsigned tintColor) const
{
	draw(dst, dstX, dstY, 0, 0, _width, _height, tintColor);
}

void Texture::clear(unsigned char r, unsigned char g, unsigned char b)
{
	for(unsigned x = _x; x < _x + _width; x++) {
		for(unsigned y = _y; y < _y + _height; y++) {
			_data[(x + y * (int) _dataWidth) * 3 + 0] = b;
			_data[(x + y * (int) _dataWidth) * 3 + 1] = g;
			_data[(x + y * (int) _dataWidth) * 3 + 2] = r;
		}
	}
}

void Texture::clearBlack()
{
	memset(_data, 0xff, _dataWidth * _dataHeight * 3);
}

void Texture::setPixel (int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	if(x >= 0 && y >= 0 && x < (int) _width && y < (int)  _height) {
		if(r == (keyColor & 0xff) && g == ((keyColor >> 8) & 0xff) && b == ((keyColor >> 16) & 0xff))
			return;
		
		setPixelFast(x, y, r, g, b);
	}
}

void Texture::setPixelFast(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	_data[(x + _x + ((int) _dataHeight - y - _y - 1) * (int) _dataWidth) * 3 + 0] = b;
	_data[(x + _x + ((int) _dataHeight - y - _y - 1) * (int) _dataWidth) * 3 + 1] = g;
	_data[(x + _x + ((int) _dataHeight - y - _y - 1) * (int) _dataWidth) * 3 + 2] = r;
}

void Texture::setPixelFastInvY(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	_data[(x + _x + (y + _y) * (int) _dataWidth) * 3 + 0] = b;
	_data[(x + _x + (y + _y) * (int) _dataWidth) * 3 + 1] = g;
	_data[(x + _x + (y + _y) * (int) _dataWidth) * 3 + 2] = r;
}

const unsigned char* Texture::pixelPtr(int x, int y) const
{
	return &_data[(x + _x + ((int) _dataHeight - y - _y - 1) * (int) _dataWidth) * 3];
}

unsigned char* Texture::pixelPtr(int x, int y)
{
	return &_data[(x + _x + ((int) _dataHeight - y - _y - 1) * (int) _dataWidth) * 3];
}

unsigned int Texture::pixel(int x, int y) const
{
	unsigned result = unsigned(0x00) | 
	unsigned(_data[(x + _x + ((int) _dataHeight - y - _y - 1) * (int) _dataWidth) * 3 + 0]) << 16 |
	unsigned(_data[(x + _x + ((int) _dataHeight - y - _y - 1) * (int) _dataWidth) * 3 + 1]) << 8 |
	unsigned(_data[(x + _x + ((int) _dataHeight - y - _y - 1) * (int) _dataWidth) * 3 + 2]) << 0;
	
	return result;
}
