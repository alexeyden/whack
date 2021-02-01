#ifndef GUNMANAGER_H
#define GUNMANAGER_H

#include <vector>

#include "guns.h"

class Entity;
class TextureAtlas;

class GunManager
{
public:	
	enum class GunState : uint8_t {
		GS_Idle = 0,
		GS_Firing = 1,
		GS_Reloading = 2,
		GS_Changing = 3,
		
		GunStateMax = GS_Changing
	};

public:
	GunManager(Entity* owner);
	
	void fire();
	void reload();
	
	void update(float dt);
	
	void put(const Gun& gun);
	void remove(GunType id);
	bool has(GunType id);
	const Gun& get(GunType id) const;
	
	GunType gunType() const {
		return _gun;
	}
	
	const Gun& gun() const {
		return _guns[static_cast<uintmax_t>(_gun)];
	}
	
	Gun& gun() {
        return _guns[static_cast<uintmax_t>(_gun)];
    }
    
    void gun(const GunType& id) {
		_gun = id;
	}
	
	GunState state() const {
		return _state;
	}
	
	float timeout() const {
		return _timeout;
	}
	
private:
	void traceShoot();
	
	std::vector<Gun> _guns;
	GunType _gun;
	GunState _state;
	float _timeout;
	
	const TextureAtlas* _atlas;
	
	const Entity* _owner;
	
	const GunType c_default_gun;
};

#endif // GUNMANAGER_H
