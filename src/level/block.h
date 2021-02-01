#ifndef BLOCK_H
#define BLOCK_H

#include "graphics/animation.h"
#include "util/math.h"

#include <vector>
#include <memory>

enum BlockTex {
	BlockTex_FIRST = 0,
	
	BT_FLOOR = 0,
	BT_CEILING = 1,
	BT_NORTH = 2,
	BT_SOUTH = 3,
	BT_WEST = 4,
	BT_EAST = 5,
	
	BlockTex_SIZE
};

struct BlockDecal {
	float offsetX;
	float offsetY;
	float sizeX;
	float sizeY;
	unsigned texture;
	bool blend;
};

class BlockAnimationManager {
public:
	BlockAnimationManager();
	
	unsigned add(const Animation& anim);
	unsigned frame(unsigned id) const;
	const Animation& animation(unsigned id) const;
	
	void update(double dt);
private:
	const double _frameTime = 1.0/15.0;
	double _time;
	std::vector<std::pair<unsigned, Animation>> _frames;
};

class Block
{
public:
	Block();
	Block(float height, unsigned tex);
	
	float height;
	
	float notch;
	float notchHeight;
	
	bool liquid;
	
	struct {
		uint32_t floor[3], ceiling[3];
		uint32_t north_bot[3], south_bot[3], east_bot[3], west_bot[3];
		uint32_t north_top[3], south_top[3], east_top[3], west_top[3];
	} lightingTint;
	
	unsigned texture(BlockTex side) const;
	void texture(BlockTex side, unsigned staticId);
	void texture(BlockTex side, const Animation& anim);
	
	const std::vector<BlockDecal>& decals(BlockTex side) const { return _decals[side]; }
	std::vector<BlockDecal>& decals(BlockTex side) { return _decals[side]; }
	
	bool isAnimated(BlockTex side) const;
	const Animation* animation(BlockTex side) const;
	
	static void update(double dt);
	
	bool operator==(const Block& op) const;
	
	float standHeight() const {
		if (liquid)
			return std::max(standHeightVisual() - liquid_gauge, 0.0f);
		
		return standHeightVisual();
	}
	
	float standHeightVisual() const {
		return (notchHeight == 0) ? height : notch;
	}
	
	bool hasTop() const {
		return notchHeight != 0;
	}
	
	AABB<float> bottomAABB(int x, int y) const {
		if(hasTop()) {
			return AABB<float>(x, y, 0, x+1, y+1, notch);
		}
		else {
			return AABB<float>(x, y, 0, x+1, y+1, height);
		}
	}
	
	AABB<float> topAABB(int x, int y) const {
		return AABB<float>(x, y, notch + notchHeight, x + 1, y + 1, height);
	}
	
	bool tracerVisit;
private:
	static constexpr const float liquid_gauge = 0.6f;
	static BlockAnimationManager animationManager;
	
	std::vector<BlockDecal> _decals[6];
	uint32_t textures[6];
	uint8_t animationBits;
};

#endif // BLOCK_H
