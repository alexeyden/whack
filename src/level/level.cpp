#include "level.h"

#include <cstring>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <map>

#include "util/util.h"

#include "levelraycaster.h"
#include "staticskydome.h"
#include "lightingcalculator.h"

Level::Level(unsigned width, unsigned height, const Block& proto, Backend *be) : 
	_sizeX(width), _sizeY(height)
{
	_data = new Block[width * height];
	
	for (unsigned i = 0; i < width*height; ++i) {
		_data[i] = proto;
	}
	
	init(be);
}

Level::Level(std::string name, unsigned int width, unsigned int height, Block* data, Backend *be)
	: name(name), description(""), _sizeX(width), _sizeY(height), _data(data)
{
	init(be);
}

void Level::init(Backend *be)
{
	_physics = new PhysicsManager(this);
	_player = new Player(10.0f, 10.0f, 3.0f);
	_player->level = this;
	
	_sky = new StaticSkydome("cloudy");
	
	_entities = new UnitGrid<VisualEntity*>(_sizeY);
	
	enablePhysics = true;
	
	_pathfinderThread = new JobRunner<std::list<point2i>>(be, "path_finder");

	clampEdges = true;
}

Level::~Level()
{
	delete _pathfinderThread;
	
	delete [] _data;
	
	for(VisualEntity* e : _entities->allItems())
		delete e;
	
	delete _entities;
	
	delete _physics;
	delete _player;
}

Block& Level::block(unsigned int x, unsigned int y)
{
	return _data[y * _sizeY + x];
}

const Block& Level::block(unsigned x, unsigned y) const {
	return _data[y * _sizeY + x];
}

const Block& Level::block(unsigned int i) const
{
	return _data[i];
}

void Level::update(double dt)
{
	static float time = 0;
	
	Block::update(dt);
	
	updatePlayer(dt);
	updateEntities(dt);
	
	time += dt;
}

Level::HitState Level::hits(double x, double y, double z, double h) const
{
	const double stage_max = 0.2;
	
	if(x < 0 || y < 0 || x >= _sizeX || y >= _sizeY)
		return HitState::HIT;
	
	const Block& b = block(int(x), int(y));
	
	const double eq_eps = 0.001;
	if(fabs(z - b.height) < eq_eps || fabs(z - b.notch) < eq_eps) {
		return HitState::EDGE;
	}
	
	if((b.height - z) > 0 && (b.height - z) < stage_max)
		return HitState::HIT_STAGE;
	
	bool air = (
		z > b.height ||
		(z > b.notch && (z + h) < b.notch + b.notchHeight)
	);
	
	return air ? HitState::AIR : HitState::HIT;
}

void Level::updatePlayer(double dt)
{
	_player->update(dt);
	
	if(enablePhysics)
		_physics->handle(_player, dt);
}

void Level::updateEntities(double dt)
{
	float px,py; //old position
	
	std::vector<VisualEntity*> to_delete;
	
	for(VisualEntity* e : _entities->allItems()) {
		px = e->x(); py = e->y(); 
		
		e->update(dt);
		
		if(enablePhysics)
			_physics->handle(e, dt);
		
		if(e->remove)
			to_delete.push_back(e);
		
		_entities->move(e, px, py, e->x(), e->y(), e->visualWidth());
	}
	
	for(VisualEntity* e : to_delete)
		_entities->remove(e, e->x(), e->y(), e->visualWidth());
}

void Level::addEntity(VisualEntity* e)
{
	_entities->add(e, e->x(), e->y(), e->visualWidth());
	e->level = this;
}

void Level::moveEntity(VisualEntity* e, float ex, float ey, float ez)
{
	_entities->move(e, e->x(), e->y(), ex, ey, e->visualWidth());
	e->x(ex);
	e->y(ey);
	e->z(ez);
}

void Level::removeEntity(VisualEntity* e)
{
	_entities->remove(e, e->x(), e->y(), e->visualWidth());
}

const std::vector< VisualEntity* >& Level::entitiesAt(unsigned int x, unsigned int y) const
{
	return _entities->cellItems(x, y);
}

void Level::addLight(const Light& light)
{
	_lights.push_back(light);
}

void Level::removeLight(const Light& light)
{
	_lights.erase(std::find(_lights.begin(), _lights.end(), light));
}

void Level::updateLights()
{
	LightingCalculator calc(this);
	calc.calculate();
}

std::future<std::list<point2i>> Level::findPath(const point2i& src, const point2i& dst)
{
	return _pathfinderThread->run(new PathFinderJob(this, src, dst));
}
