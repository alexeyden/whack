#include "levelraycaster.h"

#include "level.h"
#include "objects/visualentity.h"
#include "objects/player.h"
#include "resources/resources.h"

LevelRayCaster::LevelRayCaster(Level* level)
 : _level(level)
{
	maxDistance = 10.0f;
	no_entities = false;
	
	stop_target.x = -1;
	stop_target.y = -1;
}

LevelRayCaster::~LevelRayCaster()
{
	
}

CastResult LevelRayCaster::cast(const vec3& pos, const vec3& dir, bool plane)
{
	int mapX = int(pos.x);
	int mapY = int(pos.y);
	
	double sideDistX, sideDistY;
	double deltaDistX = sqrt(1 + (dir.y * dir.y) / (dir.x * dir.x));
	double deltaDistY = sqrt(1 + (dir.x * dir.x) / (dir.y * dir.y));
	
	int stepX, stepY;
	bool hitXSide;

	if(dir.x < 0) {
		stepX = -1; sideDistX = (pos.x - mapX) * deltaDistX; 
	} else {
		stepX = 1; sideDistX = (mapX + 1.0 - pos.x)  * deltaDistX;
	}
	
	if(dir.y < 0) {
		stepY = -1; sideDistY = (pos.y - mapY) * deltaDistY;
	} else {
		stepY = 1; sideDistY = (mapY + 1.0 - pos.y) * deltaDistY;
	}
	hitXSide = sideDistX < sideDistY;
	
	CastResult result;
	
	vec3 position;
	float prev_z = pos.z;
	
	while(mapX >= 0 && mapY >= 0 && mapX < (int) _level->sizeX() && mapY < (int) _level->sizeY())
	{
		Block& block = _level->block(mapX, mapY);
		
		if(hitXSide) {
			position.x = (dir.x >= 0) ? mapX : mapX + 1;
			position.y = pos.y + ((mapX - pos.x + (1 - stepX) / 2) / dir.x) * dir.y;
			position.z = pos.z + dir.z * vec2(position.x - pos.x, position.y - pos.y).length() / dir.xy().length();
		}
		else {
			position.x = pos.x + ((mapY - pos.y + (1 - stepY) / 2) / dir.y) * dir.x;
			position.y = (dir.y >= 0) ? mapY : mapY + 1;
			position.z = pos.z + dir.z * vec2(position.x - pos.x, position.y - pos.y).length() / dir.xy().length();
		}
		
		if(stop_target.x >= 0) {
			bool stop_point = (position - stop_target).length() < 0.1f;
			bool stop_plane = std::signbit(position.z - stop_target.z) != std::signbit(prev_z - stop_target.z);
			
			if(stop_point || stop_plane)
				return result;
		}
		
		if(rayAABBIntersect3D(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, _level->player()->collisionAABB())) {
			result.player = _level->player();
		}
		
		if(block.notchHeight > 0) {
			AABB<float> top = blockAABBTop(mapX, mapY, block);
			
			if(rayAABBIntersect3D(pos.x,pos.y, pos.z, dir.x, dir.y, dir.z, top) &&
				result.block == nullptr)
			{
				result.block = &block;
				result.position = position; 
				
				if(!plane)
					return result;
			}
		}
		
		AABB<float> bottom = blockAABBBottom(mapX, mapY, block);
		
		if(rayAABBIntersect3D(pos.x,pos.y, pos.z, dir.x, dir.y, dir.z, bottom) &&
			result.block == nullptr)
		{
			result.block = &block;
			result.position = position; 
			
			if(!plane)
				return result;
		}
		
		if(!no_entities)
		for(const auto entity : _level->entitiesAt(mapX, mapY)) {
			if(!(entity->Collider.Mask & ColliderInfo::MaskRays))
				continue;
		
			float t = 0;
			
			vec3 ent_pos {entity->x(), entity->y(), entity->z()};
			vec2 ent_size {entity->width() / 2.0f, entity->height()};
			
			vec3 dir_ent = dir;
			
			if(plane) {
				dir_ent = dir;
				dir_ent.z = (ent_pos.z + ent_size.y / 2.0f - pos.z)/(ent_pos.xy() - pos.xy()).length();
				dir_ent.normalize();
			}
			
			if((t = rayCylinderIntersect(dir_ent, pos, ent_pos, ent_size)) > 0) {
				result.block = nullptr;
				result.entity = entity;
				result.position = pos + dir_ent * t; 
				return result;
			}
		}

		
		if(sideDistX < sideDistY) {
				sideDistX += deltaDistX;
				mapX += stepX;
				hitXSide = true;
		}
		else {
			sideDistY += deltaDistY;
			mapY += stepY;
			hitXSide = false;
		}
		
		if(std::min(sideDistX,sideDistY) > maxDistance) {
			break;
		}
		
		prev_z = position.z;
	}

	return result;
}

AABB<float> LevelRayCaster::blockAABBTop(int x, int y, const Block& block)
{
	AABB<float> aabb;
	aabb.x0 = x;
	aabb.y0 = y;
	aabb.z0 = block.notch + block.notchHeight;
		
	aabb.x1 = x + 1.0f;
	aabb.y1 = y + 1.0f;
	aabb.z1 = block.height;
	
	return aabb;
}

AABB< float > LevelRayCaster::blockAABBBottom(int x, int y, const Block& block)
{
	AABB<float> aabb;
	aabb.x0 = x;
	aabb.y0 = y;
	aabb.z0 = 0; 
		
	aabb.x1 = x + 1.0f;
	aabb.y1 = y + 1.0f;
	aabb.z1 = (block.notchHeight == 0.0f) ? block.height : block.notch; 
	
	return aabb;
}
