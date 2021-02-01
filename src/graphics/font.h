#ifndef FONT_H
#define FONT_H

#include <string>

#include "texture.h"

class Font
{
public:
		Font(const Texture* font_tex, unsigned charSize,
				 unsigned colorText, unsigned colorShadow, unsigned outlineColor,
			 int shadowdx, int shadowdy);
    Font(const Texture* font_tex, unsigned charSize);
		Font(const Font& copy);
	
		void drawText(Texture* dst, int x, int y, const char* str, int maxwidth=0, int maxheight=0) const;
	
		std::pair<int,int> textBounds(std::string text) const;
		
		unsigned textColor;
		unsigned shadowColor;
		unsigned outlineColor;
		
		int shadowX, shadowY;
		
		unsigned charSize() const {
			return _charSize;
		}
protected:
	void drawCharWithOutline(Texture* out, int dstX, int dstY, int srcX, int srcY) const;
	
	unsigned _charSize;
	const Texture* texture;
};

#endif // FONT_H
