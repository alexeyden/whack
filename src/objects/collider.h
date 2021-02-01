#ifndef COLLIDER_H
#define COLLIDER_H

#include <cinttypes>

struct ColliderInfo {
	static const uint8_t MaskGeometry  = 0b0001;
	static const uint8_t MaskEntities  = 0b0010;
	static const uint8_t MaskRays			 = 0b0100;
	
	static const uint8_t ShapeAABB 			= 0x00;
	static const uint8_t ShapeCylinder	= 0x01;
	
	ColliderInfo() : Mask(0x00), Shape(0x00) {
	}
	
	uint8_t Mask;
	uint8_t Shape;
};

#endif