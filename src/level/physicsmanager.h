#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

class Level;
class Entity;
class Block;

class PhysicsManager
{
public:
	PhysicsManager(Level* level);
	~PhysicsManager();
	
	void handle(Entity* e, float dt);
	
private:
	struct AABB {
		float x, y, z; //center
		float sizeX, sizeY, sizeZ; //size
	};
	
	const float stair_max = 0.4f;
	const float liquid_max = 0.8f;
	
	bool resolve(AABB& object, const AABB& block, const Entity* ent);
	bool resolveCell(int x, int y, AABB& object, const Entity* ent);
	
	void handleAABB(AABB& obj, const AABB& stat);
	
	void blockAABB(int x, int y, AABB& bottom, AABB& top);
	AABB entityAABB(const Entity* e);
	
	bool valid(int x, int y);
	
	Level* _level;
};

#endif // PHYSICSMANAGER_H
