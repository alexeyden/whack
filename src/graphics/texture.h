#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <cstdint>

class Texture
{
public:
        Texture(uint8_t* data, unsigned x, unsigned y, unsigned w, unsigned h, unsigned dataW, unsigned dataH);
		Texture(); //needed for qt type registering
		
		void draw(Texture* dst, int dstX, int dstY,
							int srcX, int srcY, int srcW, int srcH, unsigned tintColor) const;
		void draw(Texture* dst, int dstX, int dstY, unsigned tintColor = 0) const;
		void drawScaled(Texture* dst,
										int dstX, int dstY, int dstW, int dstH,
										int srcX, int srcY, int srcW, int srcH,
										unsigned tintColor = 0) const;
		
		void clear(unsigned char r, unsigned char g, unsigned char b);
		void clearBlack();
		
		void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
		void setPixelFast(int x, int y, unsigned char r, unsigned char g, unsigned char b);
		void setPixelFastInvY(int x, int y, unsigned char r, unsigned char g, unsigned char b);
		
		unsigned pixel(int x, int y) const;
		const unsigned char* pixelPtr(int x, int y) const;
		unsigned char* pixelPtr(int x, int y);
	
		unsigned x() const { return _x; }
		unsigned y() const { return _y; }
		unsigned width() const { return _width; }
		unsigned height() const { return _height; }
		
		unsigned dataWidth() const { return _dataWidth; }
		unsigned dataHeight() const { return _dataHeight; }
		
		uint8_t * data() const { return _data; } 
	
		static const unsigned keyColor = 0x00ff00ff;
		
		static const unsigned char keyColorR = 0xff;
		static const unsigned char keyColorG = 0x00;
		static const unsigned char keyColorB = 0xff;
private:
	unsigned _x, _y;
	unsigned _width, _height;
	unsigned _dataWidth, _dataHeight;
	uint8_t* _data;
};

#endif // TEXTURE_H
