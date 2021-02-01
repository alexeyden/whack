#ifndef LIGHTINGCALCULATOR_H
#define LIGHTINGCALCULATOR_H

#include <stdint.h>
#include "levelraycaster.h"
#include "util/util.h"

class Level;
class vec3;
class Block;

enum LightType {
	LT_SUN = 0,
	LT_POINT = 1
};

struct Light {
	Light() {
		type = LT_SUN;
		intensity = 1.0;
		red = 0xff;
		green = 0xff;
		blue = 0xff;
		x = 0;
		y = 0;
		z = 0;
	}
	
	LightType type;
	float intensity;
	
	uint32_t red;
	uint32_t green;
	uint32_t blue;
	
	float x;
	float y;
	float z;
	
	bool operator==(const Light& that) {
		return x == that.x && y == that.y && z == that.z;
	}
};

class LightingCalculator
{
public:
	LightingCalculator(Level* level) :
	_caster(level), _level(level)
    {
		ambientRGB[0] = 0x30;
		ambientRGB[1] = 0x30;
		ambientRGB[2] = 0x30;
		
		_caster.maxDistance = 10000.0f;
		_caster.no_entities = true;
	}
	
	void calculate();

	uint32_t ambientRGB[3];
protected:
	void preparePass();
	void calculatePass();
	void calcLight(const Light& light, Block& block, uint32_t x, uint32_t y);
	void averagePass();
	void averageFloor();
	void averageNorthSouth();
	void averageEastWest();
	void spritesPass();

	bool isBlockBottomVisible(uint32_t x, uint32_t y, Dir dir) const;
	bool isBlockTopVisible(uint32_t x, uint32_t y, Dir dir) const;
	
	void lightAdd(uint32_t (&color)[3], const Light& light, float distance);
	bool hitsLight(const vec3& light, const vec3& point);
	
	LevelRayCaster _caster;
	Level* _level;
};

#endif // LIGHTINGCALCULATOR_H
