#ifndef LEVELRAYCASTER_H
#define LEVELRAYCASTER_H

#include "util/math.h"

class VisualEntity;
class Player;
class Block;
class Level;

struct CastResult {
	CastResult() :
		entity(nullptr),
		player(nullptr),
		block(nullptr) {}
	
	VisualEntity* entity;
	Player* player;
	Block* block;
	
	bool is_hit() {
		return entity != nullptr || player != nullptr || block != nullptr;
	}
	
	vec3 position;
};

class LevelRayCaster
{
public:
	LevelRayCaster(Level* level);
	~LevelRayCaster();

	CastResult cast(const vec3& pos, const vec3& dir, bool plane = false);
	
	float maxDistance;
	
	bool no_entities;
	vec3 stop_target;
private:
	AABB<float> blockAABBTop(int x, int y, const Block& block);
	AABB<float> blockAABBBottom(int x, int y, const Block& block);
	
	Level* _level;
};

#endif // LEVELRAYCASTER_H
