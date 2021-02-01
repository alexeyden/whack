#ifndef MAP_H
#define MAP_H

#include <cmath>
#include <vector>
#include <list>
#include <utility>
#include <fstream>
#include <future>

#include "graphics/texture.h"
#include "graphics/textureatlas.h"

#include "objects/player.h"
#include "objects/entity.h"
#include "objects/visualentity.h"

#include "block.h"
#include "physicsmanager.h"
#include "skydome.h"
#include "levelraycaster.h"

#include "util/unitgrid.h"
#include "pathfinderjob.h"

class CastResult;
class Light;
class Backend;

class Level
{
public:
	enum HitState {
		AIR = -1, EDGE = 0, HIT = 1, HIT_STAGE = 2
	};
	
	Level(unsigned int width, unsigned int height, const Block& proto, Backend *be);
	Level(std::string name, unsigned width, unsigned height, Block* data, Backend *be);
	virtual ~Level();
	
	Block& block(unsigned int x, unsigned int y);
	const Block& block(unsigned int x, unsigned int y) const;
	const Block& block(unsigned i) const;
	
	HitState hits(double x, double y, double z, double h) const;
	
	void sky(SkyDome* newsky) { _sky = newsky; }
	SkyDome* sky() { return _sky; }
	
	const Player* player() const {
		return _player;
	}
	
	Player* player() {
		return _player;
	}
	
	unsigned sizeX() const {
		return _sizeX;
	}
	unsigned sizeY() const {
		return _sizeY;
	}
	
	void update(double dt);
	
	std::string name;
	std::string description;
	
	void addEntity(VisualEntity* e);
	void moveEntity(VisualEntity* e, float ex, float ey, float ez);
	void removeEntity(VisualEntity* e);
	
	const std::vector<VisualEntity*>& entitiesAt(unsigned x, unsigned y) const;
	const std::vector<VisualEntity*>& entities() const { return _entities->allItems(); }
	
	void addLight(const Light& light);
	void removeLight(const Light& light);
	void updateLights();
	
	const std::vector<Light>& lights() const { return _lights; }
	std::vector<Light>& lights() { return _lights; }
	
	std::future<std::list<point2i>> findPath(const point2i& src, const point2i& dst);
	CastResult castRay(vec3 pos, vec3 dir);
	
	bool enablePhysics;
	bool clampEdges;
private:
	
	void init(Backend *be);
	
	void updatePlayer(double dt);
	void updateEntities(double dt);
	
	unsigned _sizeX;
	unsigned _sizeY;
	
	PhysicsManager* _physics;
	UnitGrid<VisualEntity*>* _entities;
	std::vector<Light> _lights;
	
	SkyDome* _sky;
	Block* _data;
	Player* _player;
	
	JobRunner<std::list<point2i>>* _pathfinderThread;
};

#endif // MAP_H
