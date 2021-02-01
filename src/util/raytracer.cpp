#include "raytracer.h"
#include <cmath>

#include "util/util.h"
#include "util/math.h"

RayTracer::RayTracer(Level* level, EntityFilter* filter, const unsigned int maxHits, float maxDist) :
	_maxHits(maxHits), _maxDist(maxDist), _level(level), _filter(filter)
{
	_hits.reserve(_maxHits);
}

RayTracer::~RayTracer()
{
}

const std::vector<TraceInfo>& RayTracer::trace(double posX, double posY, double dirX, double dirY, bool collectEntities = false)
{
	_hits.clear();
	_filter->rayStart = vec2(posX, posY);
	_filter->rayDir = vec2(dirX, dirY);
	
	int mapX = int(posX);
	int mapY = int(posY);
	
	double sideDistX, sideDistY;
	double deltaDistX = sqrt(1 + (dirY * dirY) / (dirX * dirX));
	double deltaDistY = sqrt(1 + (dirX * dirX) / (dirY * dirY));
	
	int stepX, stepY;
	bool hitXSide;
	bool empty = false;

	if(dirX < 0) {
		stepX = -1; sideDistX = (posX - mapX) * deltaDistX; 
	} else {
		stepX = 1; sideDistX = (mapX + 1.0 - posX)  * deltaDistX;
	}
	
	if(dirY < 0) {
		stepY = -1; sideDistY = (posY - mapY) * deltaDistY;
	} else {
		stepY = 1; sideDistY = (mapY + 1.0 - posY) * deltaDistY;
	}
	hitXSide = sideDistX < sideDistY;

	float prev_height, prev_notch, prev_notch_height;
	
	bool max_dist_reached = false;
	
	while(mapX >= 0 && mapY >= 0 &&
        mapX < (int) _level->sizeX() && mapY < (int) _level->sizeY() && !max_dist_reached)
	{
		TraceInfo hitInfo;
		const Block& block = _level->block(mapX, mapY);
	
		if(hitXSide == true) {
			hitInfo.start.distance = fabs((mapX - posX + (1 - stepX)/2) / dirX);
			hitInfo.start.wallDisp = posY + ((mapX - posX + (1 - stepX) / 2) / dirX) * dirY;
		}
		else {
			hitInfo.start.distance = fabs((mapY - posY + (1 - stepY)/2) / dirY);
			hitInfo.start.wallDisp = posX + ((mapY - posY + (1 - stepY) / 2) / dirY) * dirX;
		}
		
		if(mapX == int(posX) && mapY == int(posY))
			hitInfo.start.distance = 0.5;
		
		hitInfo.start.wallDisp = hitInfo.start.wallDisp - floor(hitInfo.start.wallDisp);
		
		if(hitXSide == true && dirX >= 0)
			hitInfo.start.hitSide = DIR_W;
		else if(hitXSide == true && dirX < 0)
			hitInfo.start.hitSide = DIR_E;
		else if(hitXSide == false && dirY >= 0)
			hitInfo.start.hitSide = DIR_S;
		else
			hitInfo.start.hitSide = DIR_N;
		
		hitInfo.start.x = mapX;
		hitInfo.start.y = mapY;
	
		prev_height = block.height;
		prev_notch = block.notch;
		prev_notch_height = block.notchHeight;
		
		if(collectEntities &&
			_filter->includePlayer &&
			_filter->filter(_level->player()))
		{
			_hitEntities.insert(_level->player());
		}
		
		while(true) {
			if(collectEntities) {
				for(VisualEntity* e : _level->entitiesAt(mapX, mapY)) {
					if(_hitEntities.find(e) != _hitEntities.end())
						continue;
					else if(_filter->filter(e)) {
						_hitEntities.insert(e);
					}
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
			
			if(mapX < 0 || mapY < 0 || mapX >= (int) _level->sizeX() || mapY >= (int) _level->sizeY()) {
				empty = true;
				break;
			}
			
			const Block& cur_block = _level->block(mapX, mapY);
			if(cur_block.height != prev_height || cur_block.notch != prev_notch ||
				cur_block.notchHeight != prev_notch_height)
			{
				break;
			}
			
			if(std::min(sideDistX,sideDistY) > _maxDist) {
				max_dist_reached = true;
				break;
			}
		}
		
		if(hitXSide == true) {
			hitInfo.end.distance = fabs((mapX - posX + (1.0 - stepX)/2.0) / dirX);
			hitInfo.end.wallDisp = posY + ((mapX - posX + (1.0 - stepX) / 2.0) / dirX) * dirY;
		}
		else {
			hitInfo.end.distance = fabs((mapY - posY + (1.0 - stepY)/2.0) / dirY);
			hitInfo.end.wallDisp = posX + ((mapY - posY + (1.0 - stepY) / 2.0) / dirY) * dirX;
		}
		hitInfo.end.wallDisp = hitInfo.end.wallDisp - floor(hitInfo.end.wallDisp);
		
		if(hitXSide == true && dirX >= 0)
			hitInfo.end.hitSide = DIR_W;
		else if(hitXSide == true && dirX < 0)
			hitInfo.end.hitSide = DIR_E;
		else if(hitXSide == false && dirY >= 0)
			hitInfo.end.hitSide = DIR_S;
		else
			hitInfo.end.hitSide = DIR_N;
		
		hitInfo.end.x = mapX;
		hitInfo.end.y = mapY;
		
		hitInfo.hitBorder = empty;
		
		_hits.push_back(hitInfo);
		if(_hits.size() > _maxHits) {
			return _hits;
		}
	}
	
	return _hits;
}
