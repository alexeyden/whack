#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

const unsigned short NullTexID = 7;

enum Dir 
{
	DIR_N = 0,
	DIR_S = 1,
	DIR_W = 2,
	DIR_E = 3,
	DIR_NW = 4,
	DIR_NE = 5,
	DIR_SE = 6, 
	DIR_SW = 7
};

inline unsigned char colorR(unsigned colorBGR) {
	return colorBGR & 0xff;
}

inline unsigned char colorG(unsigned colorBGR) {
	return (colorBGR >> 8) & 0xff;
}

inline unsigned char colorB(unsigned colorBGR) {
	return (colorBGR >> 16) & 0xff;
}

inline unsigned colorBGR(unsigned char r, unsigned char g, unsigned char b) {
	return r | unsigned(g) << 8 | unsigned(b) << 16;
}

inline uint32_t vec2rgb(const uint32_t *color) {
	uint32_t rgb = 0;
	rgb = (uint8_t) color[0];
	rgb |= color[1] << 8;
	rgb |= color[2] << 16;
	return rgb;
}

void strip(std::string& s, char ch);

std::vector<std::string> split(std::string str, std::string delim);


#endif // UTIL_H
