#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <vector>
#include <set>
#include <memory>

#include "level/level.h"
#include "util/math.h"

struct TraceHit {
	unsigned char hitSide;
	double distance;
	double wallDisp;
	int x, y;
};

struct TraceInfo {
	TraceHit start;
	TraceHit end;
	
	bool hitBorder; //if ray is out of the level bounds
};

struct EntityFilter {
	EntityFilter() : includePlayer(false) {}
	
	bool includePlayer;
	vec2 rayDir;
	vec2 rayStart;
	
	virtual bool filter(const Entity* e) const = 0;
};

struct EmptyEntityFilter : EntityFilter {
	virtual bool filter(const Entity* e) const override {
        (void) e;
        
		return true;
	}
};

struct VisBoxRayHitEntityFilter : EntityFilter {
	virtual bool filter(const Entity* e) const override {
		if(e != e->level->player())
			return rayAABBIntersect2D(rayStart.x, rayStart.y, rayDir.x, rayDir.y, ((VisualEntity*) e)->visualAABB());
		return false;
	}
};

struct ColBoxRayHitEntityFilter : EntityFilter {
	virtual bool filter(const Entity* e) const override {
		return rayAABBIntersect2D(rayStart.x, rayStart.y, rayDir.x, rayDir.y, e->collisionAABB());
	}
};

class RayTracer
{
public:
	RayTracer(Level* level, EntityFilter* filter, const unsigned int maxHits = 8, float maxDist = 8.0f);
	~RayTracer();
	
	const std::vector<TraceInfo>& trace(double posX, double posY, double dirX, double dirY, bool collectEntities);
	
	const std::set<Entity*>& hitEntities() const { return _hitEntities; }
	const std::vector<TraceInfo>& hits() const { return _hits; }
	
	void reset() {
		_hitEntities.clear();
	}
	
private:
	unsigned _maxHits;
	float _maxDist;
	
	Level* _level;
	std::set<Entity*> _hitEntities;
	std::vector<TraceInfo> _hits;
	
	EntityFilter* _filter;
};

#endif // RAYTRACER_H
