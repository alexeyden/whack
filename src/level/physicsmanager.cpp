#include "physicsmanager.h"

#include "level.h"

#include <cfloat>

PhysicsManager::PhysicsManager(Level* level) :
	_level(level)
{
}

PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::handleAABB(PhysicsManager::AABB& obj, const PhysicsManager::AABB& stat)
{
	float minkX = (obj.x - obj.sizeX/2.0f) - (stat.x + stat.sizeX/2.0f);
	float minkY = (obj.y - obj.sizeY/2.0f) - (stat.y + stat.sizeY/2.0f);
	float minkZ = (obj.z - obj.sizeZ/2.0f) - (stat.z + stat.sizeZ/2.0f);
	float minkSX = obj.sizeX + stat.sizeX;
	float minkSY = obj.sizeY + stat.sizeY;
	float minkSZ = obj.sizeZ + stat.sizeZ;
		
	//contains origin?
	if(minkX <= 0 && minkX + minkSX > 0 &&
		 minkY <= 0 && minkY + minkSY > 0 &&
		 minkZ <= 0 && minkZ + minkSZ > 0
	) {
		//find penetration vector
		const float edgeEps = 0.001;
		
		float minDist = fabs(minkX);
		float pX = minkX - edgeEps;
		float pY = 0, pZ = 0;
			
		if(fabs(minkX + minkSX) < minDist) {
			minDist = fabs(minkX + minkSX);
			pX = minkX + minkSX + edgeEps;
			pY = pZ = 0;
		}
		
		if(fabs(minkY) < minDist) {
			minDist = fabs(minkY);
			pY = minkY - edgeEps;
			pX = pZ = 0;
		}
		if(fabs(minkY + minkSY) < minDist) {
			minDist = fabs(minkY + minkSY);
			pY = minkY + minkSY + edgeEps;
			pX = pZ = 0;
		}
		
		if(fabs(minkZ) < minDist) {
			minDist = fabs(minkZ);
			pZ = minkZ - edgeEps;
			pX = pY = 0;
		}
		if(fabs(minkZ + minkSZ) < minDist) {
			minDist = fabs(minkZ + minkSZ);
			pZ = minkZ + minkSZ + edgeEps;
			pX = pY = 0;
		}
	
		obj.x = obj.x - pX;
		obj.y = obj.y - pY;
		obj.z = obj.z - pZ;
	}
}

void PhysicsManager::blockAABB(int x, int y, PhysicsManager::AABB& bottom, PhysicsManager::AABB& top)
{
	if(x <= 0 || y <= 0 || x >= (int) _level->sizeX()-1 || y >= (int) _level->sizeY()-1) {
		bottom.x = x + 0.5f;
		bottom.y = y + 0.5f;
		bottom.z = 10;
		
		bottom.sizeX = 1.0f;
		bottom.sizeY = 1.0f;
		bottom.sizeZ = 20;
		
		//no top block
		top.x = top.y = top.z = 0;
		top.sizeX = top.sizeY = top.sizeZ = -1;
		
		return;
	}
	
	const Block& block = _level->block(x, y);
	
	if(block.notchHeight > 0) {
		bottom.x = x + 0.5f;
		bottom.y = y + 0.5f;
		bottom.z = block.notch / 2.0f;
		
		bottom.sizeX = 1.0f;
		bottom.sizeY = 1.0f;
		bottom.sizeZ = block.notch;
		
		top.x = x + 0.5f;
		top.y = y + 0.5f;
		top.z = (block.height - block.notch - block.notchHeight) / 2.0f + block.notch + block.notchHeight;
		
		top.sizeX = 1.0f;
		top.sizeY = 1.0f;
		top.sizeZ = block.height - block.notch - block.notchHeight;
	}
	else {
		bottom.x = x + 0.5f;
		bottom.y = y + 0.5f;
		bottom.z = block.standHeight() / 2.0f;
		
		bottom.sizeX = 1.0f;
		bottom.sizeY = 1.0f;
		bottom.sizeZ = block.standHeight();
		
		//no top block
		top.x = top.y = top.z = 0;
		top.sizeX = top.sizeY = top.sizeZ = -1;
	}
}

PhysicsManager::AABB PhysicsManager::entityAABB(const Entity* e)
{
	return AABB {
		e->x(), e->y(), e->z() + e->height() / 2.0f,
		 
		e->width(),
		e->width(),
		e->height()
	};
}

void PhysicsManager::handle(Entity* e, float dt)
{
	AABB object = { 
		e->x(), e->y(), e->z() + e->height()/2.0f,
		e->width(), e->width(), e->height()
	};
	
	bool on_stair = false;
	
	on_stair |= resolveCell(e->x(), e->y(), object, e);
	
	if(fmod(e->x(), 1) <= 0.5 && fmod(e->y(), 1) <= 0.5) {
		on_stair |= resolveCell(e->x() - 1, e->y() - 1, object, e);
		on_stair |= resolveCell(e->x() - 1, e->y() - 0, object, e);
		on_stair |= resolveCell(e->x() - 0, e->y() - 1, object, e);
	}
	else if(fmod(e->x(), 1) <= 0.5 && fmod(e->y(), 1) > 0.5) { 
		on_stair |= resolveCell(e->x() - 1, e->y() + 1, object, e);
		on_stair |= resolveCell(e->x() - 1, e->y() - 0, object, e);
		on_stair |= resolveCell(e->x() - 0, e->y() + 1, object, e);
	}
	else if(fmod(e->x(), 1) > 0.5 && fmod(e->y(), 1) <= 0.5) {
		on_stair |= resolveCell(e->x() + 1, e->y() - 1, object, e);
		on_stair |= resolveCell(e->x() + 1, e->y() - 0, object, e);
		on_stair |= resolveCell(e->x() - 0, e->y() - 1, object, e);
	}
	else if(valid(e->x() + 1, e->y() + 1)) { // fmod(e->x(), 1) > 0.5 && fmod(e->y(), 1) > 0.5
		on_stair |= resolveCell(e->x() + 1, e->y() + 1, object, e);
		on_stair |= resolveCell(e->x() + 1, e->y() - 0, object, e);
		on_stair |= resolveCell(e->x() - 0, e->y() + 1, object, e);
	}
	
	const float eq_eps = 0.001;

	if((fabs(object.x - e->x()) > eq_eps || 
		fabs(object.y - e->y()) > eq_eps ||
		fabs(object.z - e->height()/2.0f - e->z()) > eq_eps) && !on_stair) {
		e->collision(object.x - e->x(), object.y - e->y(), object.z - e->z());
	}
	
	e->x(object.x);
	e->y(object.y);
	e->z(object.z - e->height()/2.0f);
	
	const Block& b00 = _level->block(int(e->x() - e->width()/2), int(e->y() - e->width()/2));
	const Block& b01 = _level->block(int(e->x() - e->width()/2), int(e->y() + e->width()/2));
	const Block& b10 = _level->block(int(e->x() + e->width()/2), int(e->y() - e->width()/2));
	const Block& b11 = _level->block(int(e->x() + e->width()/2), int(e->y() + e->width()/2));
	
	const float edge_eps = 0.005;
	if(
		(e->z() - b00.standHeight()) < edge_eps ||
		(e->z() - b01.standHeight()) < edge_eps ||
		(e->z() - b10.standHeight()) < edge_eps ||
		(e->z() - b11.standHeight()) < edge_eps
	) {
		e->speedZ(0);
        
        if(e->impulse) {
            e->speedXY(0, 0);
            e->impulse = false;
        }
	}
	else if(!on_stair && e->physics) {
		e->speedZ(e->speedZ() - 9.8 * dt);
	}
}

bool PhysicsManager::resolve(AABB& obj, const AABB& block, const Entity* e)
{
	if(block.sizeX < 0)
		return false;
	
	AABB old = obj;
		
	handleAABB(obj, block);
	
	bool penetratedXY = obj.x != old.x || obj.y != old.y || obj.z != old.z;
	bool stair = (block.z + block.sizeZ/2.0f) > e->z() &&
		(block.z + block.sizeZ/2.0f) < (e->z() + stair_max);
	bool liquid = (block.z + block.sizeZ/2.0f) > e->z() &&
		(block.z + block.sizeZ/2.0f) < (e->z() + liquid_max) &&
		_level->block(e->x(), e->y()).liquid;
	
	//stair climbing 
	//const float stair_k = 8.0f;
	if(penetratedXY && ((stair && e->speedZ() >= 0) || (liquid && e->speedZ() == 0))) {
		const float k_xy = (liquid ? 0.3f : 0.0f);
		const float k_z = 0.5f; 
		obj.x = old.x + (obj.x - old.x) * k_xy;
		obj.y = old.y + (obj.y - old.y) * k_xy;
		obj.z = old.z + (obj.z - old.z) * k_z + (liquid ? vec2(obj.x - old.x, obj.y - old.y).length() * 0.5f : 0.0f);
		
		return true; // is on stair
	}
	
	return false;
}

bool PhysicsManager::resolveCell(int x, int y, PhysicsManager::AABB& object, const Entity* ent)
{
	if(!valid(x, y))
		return false;
	
	AABB bottom, top;
	bool on_stair = false;
	
	if(ent->Collider.Mask & ColliderInfo::MaskGeometry) {
		blockAABB(x, y, bottom, top);
		on_stair |= resolve(object, bottom, ent) || resolve(object, top, ent);
	}
	
	if(!(ent->Collider.Mask & ColliderInfo::MaskEntities))
		return on_stair;
	
	for(const Entity* e : _level->entitiesAt(x, y)) {
		if(e != ent && e->physics && (e->Collider.Mask & ColliderInfo::MaskEntities))
			on_stair |= resolve(object, entityAABB(e), e);
	}
	
	return on_stair;
}

bool PhysicsManager::valid(int x, int y)
{
	return x >= 0 && y >= 0 && x < (int) _level->sizeX() && y < (int) _level->sizeY();
}
