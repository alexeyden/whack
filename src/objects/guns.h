#ifndef GUNS_H
#define GUNS_H

#include <cstdint>
#include <cmath>

typedef uintmax_t GunType;

struct Gun {
	const uintmax_t id;
	const uintmax_t cageCapacity;
	const uintmax_t damage;
	const float fireTime;
	const float reloadTime;
	const float angle;
	const float maxRange;
	
	uintmax_t ammo;
	
	Gun(
		uintmax_t id,
		uintmax_t cage,
		uintmax_t damage,
		uintmax_t fire_time_ms,
		uintmax_t reload_time_ms,
		uintmax_t angle,
		float maxRange
	) :
		id(id),
		cageCapacity(cage),
		damage(damage),
		fireTime(fire_time_ms / 1000.0f),
		reloadTime(reload_time_ms / 1000.0f),
		angle(angle * M_PI / 180.0f),
		maxRange(maxRange),
		ammo(0)
	{
	}

	Gun(const Gun& g) :
		id(g.id),
		cageCapacity(g.cageCapacity),
		damage(g.damage),
		fireTime(g.fireTime),
		reloadTime(g.fireTime),
		angle(g.angle),
		maxRange(g.maxRange),
		ammo(0)
	{
	}
};

const Gun Knife {0, 0, 50, 200, 0, 180, 0};
const Gun Pistol {1, 12, 20, 200, 1000, 0, 16};
const Gun Shotgun {2, 4, 50, 400, 2000, 30, 10};

const uintmax_t GunsCount = 2;

#endif
