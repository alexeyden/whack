#include "gunmanager.h"

#include "entityclassifier.h"
#include "splash.h"
#include "util/math.h"
#include "level/level.h"
#include "level/levelraycaster.h"
#include "graphics/textureatlas.h"

GunManager::GunManager(Entity* owner)
 : _atlas(Resources::i()->atlas("entities")),
	_owner(owner),
    c_default_gun(Pistol.id)
{
	_gun = c_default_gun;
	
	_guns.push_back(Pistol);
	_guns.push_back(Shotgun);
	
	_guns[c_default_gun].ammo = _guns[c_default_gun].cageCapacity * 2;
	
	_state = GunState::GS_Idle;
	_timeout = 0;
}

void GunManager::update(float dt)
{
	if(_timeout > 0) {
		_timeout -= dt;
		
		if(_timeout <= 0) {
			_state = GunState::GS_Idle;
			_timeout = 0;
		}
	}
}

void GunManager::fire()
{
	if(_state == GunState::GS_Idle) {
		if(_guns[_gun].ammo > 0) {
			_state = GunState::GS_Firing;
			_timeout = _guns[_gun].fireTime;
			_guns[_gun].ammo--;
			traceShoot();
		}
	}
}

void GunManager::traceShoot()
{
	LevelRayCaster caster(_owner->level);
	
	vec3 pos {
		_owner->x(), _owner->y(), _owner->z() + _owner->height()/2.0f
	};
	
	vec3 dir {
		cos(_owner->level->player()->dir()), sin(_owner->level->player()->dir()), 0.0f
	};
	
	auto result = caster.cast(pos, dir, true);
	
	if(result.entity != nullptr) {
		result.entity->damage(gun().damage, pos);
	}
	
	if(result.entity != nullptr || result.block != nullptr) {
		auto pos = result.position - dir * 0.1f;
		
		std::string splash_name = "sparks";
		
		if(result.entity != nullptr) {
			EntityClassifer cl;
			auto type = cl.classify(result.entity);
		
			if(type == EntityFactory::ET_ROBBER)
				splash_name = "blood";
			if(type == EntityFactory::ET_PLAYER)
				return;
		}
		
		VisualEntity* e = new Splash(_atlas, splash_name, pos.x, pos.y, pos.z, 0.8);
		_owner->level->addEntity(e);
	}
}

void GunManager::reload()
{
	if(_state == GunState::GS_Idle) {
		_state = GunState::GS_Reloading;
		_timeout = _guns[_gun].reloadTime;
	}
}

const Gun& GunManager::get(GunType id) const
{
	return _guns[id];
}

bool GunManager::has(GunType id)
{
	if(id == c_default_gun) {
		return true;
	}
	
	return _guns[id].ammo > 0;
}

void GunManager::put(const Gun& gun)
{
	_guns[gun.id].ammo += gun.ammo;
}

void GunManager::remove(GunType id)
{
	_guns[id].ammo = 0;
}
