#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <iostream>
#include <tuple>

#include "msgpack.hpp"
#include "level.h"

class Backend;

class LevelLoader
{
public:
	LevelLoader(std::iostream& inout, const TextureAtlas* tileAtlas, const TextureAtlas* entityAtlas, Backend *be);
	
	Level* load();
	void save(const Level* level);
	
	bool emptyBehaviuour;
private:
	struct MsgPackSig {
		std::string type;
		unsigned version;
		
		MSGPACK_DEFINE(type, version);
	};
	
	struct MsgPackHdr {
		std::string name;
		std::string description;
		unsigned width;
		unsigned height;
		
		MSGPACK_DEFINE(name, description, width, height);
	};
	
	struct MsgPackLight {
		float x, y, z;
		uint32_t r, g, b;
		float intensity;
		uint8_t type;
		
		MSGPACK_DEFINE(x, y, z, r, g, b, intensity, type);
	};
	
	struct MsgPackDecal {
		float offsetX;
		float offsetY;
		float sizeX;
		float sizeY;
		uint32_t texture;
		uint8_t side;
		bool blend; 
		
		MSGPACK_DEFINE(offsetX, offsetY, sizeX, sizeY, texture, side, blend);
	};
	
	struct MsgPackBlock {
		std::vector<unsigned> textures = std::vector<unsigned>(6);
		std::vector<MsgPackDecal> decals;
		
		float height;
		float notch;
		float notchHeight;
		
		std::vector<uint32_t> lights;
		
		MSGPACK_DEFINE(textures, decals, height, notch, notchHeight, lights);
	};
	
	struct MsgPackEntity {
		unsigned type;
		float x,y,z;
		float width, height;
		float visualWidth, visualHeight;
		
		float dir;
		std::string texture;
		uint32_t tintR, tintG, tintB;
		
		MSGPACK_DEFINE(type, x,y,z, visualWidth, visualHeight, width, height, dir, texture, tintR, tintG, tintB);
	};
	
	const TextureAtlas* tileAtlas;
	const TextureAtlas* entityAtlas;
	std::iostream& stream;
	
	const std::string type = "level";
	const unsigned version = 0x01;
    Backend *backend;
};

#endif // LEVELLOADER_H
