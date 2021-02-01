#include "font.h"

#include <cstring>

#include "SDL_log.h"
#include "util/util.h"

Font::Font(const Texture* font_tex, unsigned int charSize,
					 unsigned int colorText, unsigned int colorShadow, unsigned int outlineColor,
					 int shadowdx, int shadowdy) :
	textColor(colorText), shadowColor(colorShadow), outlineColor(outlineColor),
	shadowX(shadowdx), shadowY(shadowdy), _charSize(charSize), texture(font_tex)
{
}

Font::Font(const Texture* font_tex, unsigned charSize = 8) :
	 textColor(0x00ffffff), shadowColor(0x000000), outlineColor(0x000000),
	shadowX(0), shadowY(0), _charSize(charSize), texture(font_tex)
{
}

Font::Font(const Font& copy) :
	textColor(copy.textColor), shadowColor(copy.shadowColor), outlineColor(copy.outlineColor),
	shadowX(copy.shadowX), shadowY(copy.shadowY), 
	_charSize(copy._charSize), texture(copy.texture)
{}

std::pair< int, int > Font::textBounds(std::string text) const
{
	int max_len = 0, len = 0, lines = 1;
	for(size_t c = 0; c < text.size(); c++) {
		if(text[c] == '\n') {
			lines++;
			max_len = std::max<int>(max_len, len);
			len = 0;
		} else {
			len++;
		}
	}
	
	return std::pair<int, int>(max_len * _charSize, lines * _charSize);
}

void Font::drawText(Texture* dst, int x, int y, const char* str, int maxwidth, int maxheight) const
{
	int width = texture->width() / _charSize;
	
	int ci = 0;
	for(size_t c = 0; c < strlen(str); c++, ci++) {
		if(str[c] == '\n') {
			y += _charSize;
			ci = 0;
			
			if(maxheight > 0 && y > maxheight)
				break;
			
			continue;
		}
		
		if(str[c] == '\r')
			continue;
		
		int charX = str[c] % width;
		int charY = str[c] / width;
		
		if(maxwidth > 0 && ci * _charSize > (size_t) maxwidth)
			continue;
		
		if(shadowX != 0 || shadowY != 0) {
			texture->draw(
				dst,
				x + ci * _charSize + shadowX, y + shadowY,
				charX * _charSize, charY * _charSize,
				_charSize, _charSize,
				shadowColor
			);
		}
			
		texture->draw(
			dst,
			x + ci * _charSize, y,
			charX * _charSize, charY * _charSize, _charSize, _charSize,
			textColor
		);
		
		if(outlineColor != 0x00)
			drawCharWithOutline(dst, x + ci * _charSize, y, charX * _charSize, charY * _charSize);
	}
}

void Font::drawCharWithOutline(Texture* out, int dstX, int dstY, int srcX, int srcY) const
{
	for(int x = dstX - 1; x < dstX + (int) _charSize + 1; x++) {
		for(int y = dstY - 1; y < dstY + (int) _charSize + 1; y++) {
			if(x > 0 && x < (int) out->width() && y > 0 && y < (int) out->height()) {
				bool havePixelRight = false, havePixelLeft = false,
						havePixelTop = false, havePixelBottom = false;
				bool emptyPixel = true;
			
				if(x > dstX - 1 && y > dstY - 1 && x < dstX + (int) _charSize && y < dstY + (int) _charSize) 
					emptyPixel = texture->pixel(x - dstX + srcX, y - dstY + srcY) == Texture::keyColor;
				if(x < dstX + (int) _charSize - 1 && y != dstY - 1 && y != dstY + (int) _charSize)
					havePixelRight = texture->pixel(x - dstX + srcX + 1, y - dstY + srcY) != Texture::keyColor;
				if(x > dstX && y != dstY - 1 && y != dstY + (int) _charSize)
					havePixelLeft = texture->pixel(x - dstX + srcX - 1, y - dstY + srcY) != Texture::keyColor;
				if(y > dstY && x != dstX - 1 && x != dstX + (int) _charSize)
					havePixelTop = texture->pixel(x - dstX + srcX, y - dstY + srcY - 1) != Texture::keyColor;
				if(y < dstY + (int) _charSize - 1 && x != dstX - 1 && x != dstX + (int) _charSize)
					havePixelBottom = texture->pixel(x - dstX + srcX, y - dstY + srcY + 1) != Texture::keyColor;
				
				if(emptyPixel && (havePixelLeft || havePixelRight || havePixelBottom || havePixelTop)) {
					out->setPixelFast(x, y, colorR(outlineColor), colorG(outlineColor), colorB(outlineColor));
				}
			}
		}
	}
}
