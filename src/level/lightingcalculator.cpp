#include "lightingcalculator.h"
#include "entityvisualinfoextractor.h"
#include "level.h"
#include "util/math.h"
#include "util/rect.h"

void LightingCalculator::calculate()
{
	preparePass();
	calculatePass();
	averagePass();
	spritesPass();
}

void LightingCalculator::preparePass()
{
	for(unsigned i = 0; i < _level->sizeX(); i++) {
		for(unsigned j = 0; j < _level->sizeY(); j++) {
			auto& block = _level->block(i, j);
			
			memcpy(block.lightingTint.floor, ambientRGB, sizeof(uint32_t) * 3);
			memcpy(block.lightingTint.ceiling, ambientRGB, sizeof(uint32_t) * 3);
			
			memcpy(block.lightingTint.north_bot, ambientRGB, sizeof(uint32_t) * 3);
			memcpy(block.lightingTint.south_bot, ambientRGB, sizeof(uint32_t) * 3);
			memcpy(block.lightingTint.east_bot, ambientRGB, sizeof(uint32_t) * 3);
			memcpy(block.lightingTint.west_bot, ambientRGB, sizeof(uint32_t) * 3);
			
			memcpy(block.lightingTint.north_top, ambientRGB, sizeof(uint32_t) * 3);
			memcpy(block.lightingTint.south_top, ambientRGB, sizeof(uint32_t) * 3);
			memcpy(block.lightingTint.east_top, ambientRGB, sizeof(uint32_t) * 3);
			memcpy(block.lightingTint.west_top, ambientRGB, sizeof(uint32_t) * 3);
			
		}
	}
	
	for(VisualEntity* e : _level->entities()) {
		memcpy(e->lighting_tint, ambientRGB, sizeof(uint32_t) * 3);
	}
}

void LightingCalculator::calculatePass()
{
	for(const auto& light : _level->lights()) {
		for(unsigned i = 0; i < _level->sizeX(); i++) {
			for(unsigned j = 0; j < _level->sizeY(); j++) {
				auto& block = _level->block(i, j);
				
				calcLight(light, block, i, j);
			}
		}
	}
}

void LightingCalculator::calcLight(const Light& light, Block& block, uint32_t x, uint32_t y)
{
	EntityVisualInfoExtractor extr;
	vec3 light_p(light.x, light.y, light.z);
	Rect<float> rect_block(x, y, x + 1, y + 1);

	vec3 p = vec3(x + 0.5f, y + 0.5f, block.standHeightVisual() + 0.01f);
	
	if(hitsLight(light_p, p)) {
		bool has_entity = false;
		
		for(const auto& e : _level->entitiesAt(x, y)) {
			const auto& aabb = e->collisionAABB();
			bool near_ground = fabs(e->z() - block.standHeightVisual()) < 1.0f;
			bool intersects = Rect<float>(aabb.x0, aabb.y0, aabb.x1, aabb.y1).intersects(rect_block);
			
			if(near_ground && intersects && extr.extract(e).type == EntityFactory::ET_DECOR) {
				has_entity = true;
				break;
			}
		}
		
		if(!has_entity)
			lightAdd(block.lightingTint.floor, light, (light_p - p).length());
	}
	
	uint32_t (*sides_bot[4])[3] = {
		&block.lightingTint.north_bot,
		&block.lightingTint.south_bot,
		&block.lightingTint.west_bot,
		&block.lightingTint.east_bot
	};
	uint32_t (*sides_top[4])[3] = {
		&block.lightingTint.north_top,
		&block.lightingTint.south_top,
		&block.lightingTint.west_top,
		&block.lightingTint.east_top
	};
	
	vec3 points[4] = {
		vec3(x + 0.5f, y + 1.05f, 0),
		vec3(x + 0.5f, y - 0.05f, 0),
		vec3(x - 0.05f, y + 0.5f, 0),
		vec3(x + 1.05f, y + 0.5f, 0)
	};
	
	for(int i = 0; i < 4; i++) {
		for(float j = 0.0f; j <= 20; j += 0.5f) {
			vec3 p = points[i];
			p.z = std::min(block.standHeightVisual() * j / 20.0f, block.standHeightVisual() - 0.05f);
			if(hitsLight(light_p, p))  {
				lightAdd(*(sides_bot[i]), light, (light_p - p).length());
				break;
			}
		}
	}
	
	if(block.notchHeight > 0) {
		p = vec3(x + 0.5f, y + 0.5f, block.notchHeight + block.notch - 0.05f);
		if(hitsLight(light_p, p)) {
			lightAdd(block.lightingTint.ceiling, light, (light_p - p).length());
		}
		
		for(int i = 0; i < 4; i++) {
			for(float j = 0.0f; j <= 20.0f; j += 0.5f) {
				vec3 p = points[i];
				p.z = std::max(std::min(
					block.notch + block.notchHeight + (block.height - (block.notch + block.notchHeight))*j / 20.0f,
					block.height - 0.5f
				), block.notch + block.notchHeight + 0.05f);
				if(hitsLight(light_p, p)) {
					lightAdd(*(sides_top[i]), light, (light_p - p).length());
					break;
				}
			}
		}
	}
}

bool LightingCalculator::hitsLight(const vec3& light, const vec3& point)
{
//	EntityVisualInfoExtractor extr;
	vec3 ray = (light - point).normalize();
	_caster.stop_target = light;

	const auto& result = _caster.cast(point, ray);
	
	if(result.block != nullptr)
		return false;
	
	/*
	if(result.entity != nullptr && extr.extract(result.entity).type == EntityFactory::ET_DECOR)
	{
		float t;
		return !rayAABBIntersect3D(point.x, point.y, point.z, ray.x, ray.y, ray.z, result.entity->collisionAABB(), &t);
	}
	*/
	
	return true;
}

void LightingCalculator::lightAdd(uint32_t (&color)[3], const Light& light, float distance)
{
	if(light.type == LT_SUN) {
		color[0] = std::min<uint32_t>(uint32_t(color[0]) + light.red, 0xff);
		color[1] = std::min<uint32_t>(uint32_t(color[1]) + light.green, 0xff);
		color[2] = std::min<uint32_t>(uint32_t(color[2]) + light.blue, 0xff);
	}
	else {
		float k = std::max<float>(-distance * 1.0f/light.intensity + 1.0f, 0.0f);
		
		if(k > 0) {
			color[0] = std::min<uint32_t>(uint32_t(color[0] + light.red * k), 0xffu);
			color[1] = std::min<uint32_t>(uint32_t(color[1] + light.green * k), 0xffu);
			color[2] = std::min<uint32_t>(uint32_t(color[2] + light.blue * k), 0xffu);
		}
	}
}

void LightingCalculator::averagePass()
{
	averageFloor();
	//averageFloor();
	averageNorthSouth();
	averageEastWest();
}

void LightingCalculator::averageFloor()
{
	struct ColorU32 {
		uint32_t red;
		uint32_t green;
		uint32_t blue;
	};
	
	ColorU32* temp = new ColorU32[_level->sizeX() * _level->sizeY()];
	
	for(unsigned i = 0; i < _level->sizeX(); i++) {
		for(unsigned j = 0; j < _level->sizeY(); j++) {
			auto& block = _level->block(i, j);
			
			uint32_t tint[3];
			tint[0] = block.lightingTint.floor[0];
			tint[1] = block.lightingTint.floor[1];
			tint[2] = block.lightingTint.floor[2];
			unsigned average_n = 1;
			
			if(i > 0 && _level->block(i - 1, j).standHeight() == block.standHeight()) {
				tint[0] += _level->block(i - 1, j).lightingTint.floor[0];
				tint[1] += _level->block(i - 1, j).lightingTint.floor[1];
				tint[2] += _level->block(i - 1, j).lightingTint.floor[2];
				
				average_n += 1;
			}
			if(j > 0 && _level->block(i, j-1).standHeight() == block.standHeight()) {
				tint[0] += _level->block(i, j-1).lightingTint.floor[0];
				tint[1] += _level->block(i, j-1).lightingTint.floor[1];
				tint[2] += _level->block(i, j-1).lightingTint.floor[2];
				
				average_n += 1;
			}
			if(i < _level->sizeX()-1 && _level->block(i + 1, j).standHeight() == block.standHeight()) {
				tint[0] += _level->block(i + 1, j).lightingTint.floor[0];
				tint[1] += _level->block(i + 1, j).lightingTint.floor[1];
				tint[2] += _level->block(i + 1, j).lightingTint.floor[2];
				
				average_n += 1;
			}
			if(j < _level->sizeY()-1 && _level->block(i, j+1).standHeight() == block.standHeight()) {
				tint[0] += _level->block(i, j+1).lightingTint.floor[0];
				tint[1] += _level->block(i, j+1).lightingTint.floor[1];
				tint[2] += _level->block(i, j+1).lightingTint.floor[2];
				
				average_n += 1;
			}
			
			if(i > 0 && j > 0 && _level->block(i - 1, j-1).standHeight() == block.standHeight()) {
				tint[0] += _level->block(i - 1, j-1).lightingTint.floor[0];
				tint[1] += _level->block(i - 1, j-1).lightingTint.floor[1];
				tint[2] += _level->block(i - 1, j-1).lightingTint.floor[2];
				
				average_n += 1;
			}
			if(i > 0 && j < _level->sizeY()-1 && _level->block(i - 1, j+1).standHeight() == block.standHeight()) {
				tint[0] += _level->block(i - 1, j+1).lightingTint.floor[0];
				tint[1] += _level->block(i - 1, j+1).lightingTint.floor[1];
				tint[2] += _level->block(i - 1, j+1).lightingTint.floor[2];
				
				average_n += 1;
			}
			if(i < _level->sizeX()-1 && j > 0 && _level->block(i + 1, j-1).standHeight() == block.standHeight()) {
				tint[0] += _level->block(i + 1, j-1).lightingTint.floor[0];
				tint[1] += _level->block(i + 1, j-1).lightingTint.floor[1];
				tint[2] += _level->block(i + 1, j-1).lightingTint.floor[2];
				
				average_n += 1;
			}
			if(i < _level->sizeX()-1 && j < _level->sizeY()-1 && _level->block(i + 1, j+1).standHeight() == block.standHeight()) {
				tint[0] += _level->block(i + 1, j+1).lightingTint.floor[0];
				tint[1] += _level->block(i + 1, j+1).lightingTint.floor[1];
				tint[2] += _level->block(i + 1, j+1).lightingTint.floor[2];
				
				average_n += 1;
			}
			
			tint[0] = tint[0]/average_n;
			tint[1] = tint[1]/average_n;
			tint[2] = tint[2]/average_n;
			
			temp[i + j * _level->sizeX()] = ColorU32 {
				uint8_t(tint[0]),
				uint8_t(tint[1]),
				uint8_t(tint[2])
			};
			
		}
	}
	
	for(uint32_t i = 0; i < _level->sizeX(); i++) {
		for(uint32_t j = 0; j < _level->sizeY(); j++) {
			Block& block = _level->block(i, j);
			const ColorU32& color = temp[i + _level->sizeX() * j];
			block.lightingTint.floor[0] = color.red;
			block.lightingTint.floor[1] = color.green;
			block.lightingTint.floor[2] = color.blue;
		}
	}
	
	delete [] temp;
}

void LightingCalculator::averageEastWest()
{
	for(unsigned i = 0; i < _level->sizeX(); i++) {
		for(unsigned j = 0; j < _level->sizeY(); j++) {
			auto& block = _level->block(i, j);
			
			uint32_t tint_north_bot[3];
			uint32_t tint_south_bot[3];
			uint32_t tint_north_top[3];
			uint32_t tint_south_top[3];
			
			tint_north_bot[0] = block.lightingTint.north_bot[0];
			tint_north_bot[1] = block.lightingTint.north_bot[1];
			tint_north_bot[2] = block.lightingTint.north_bot[2];
			
			tint_south_bot[0] = block.lightingTint.south_bot[0];
			tint_south_bot[1] = block.lightingTint.south_bot[1];
			tint_south_bot[2] = block.lightingTint.south_bot[2];
			
			tint_north_top[0] = block.lightingTint.north_top[0];
			tint_north_top[1] = block.lightingTint.north_top[1];
			tint_north_top[2] = block.lightingTint.north_top[2];
			
			tint_south_top[0] = block.lightingTint.south_top[0];
			tint_south_top[1] = block.lightingTint.south_top[1];
			tint_south_top[2] = block.lightingTint.south_top[2];
			
			uint8_t average_n[4] = {1, 1, 1, 1};
			
			const float height_diff = 3.0f;
			if(i > 0 && isBlockBottomVisible(i - 1, j, DIR_N) &&
					fabs(_level->block(i - 1, j).height - block.height) < height_diff) {
				tint_north_bot[0] += _level->block(i - 1, j).lightingTint.north_bot[0];
				tint_north_bot[1] += _level->block(i - 1, j).lightingTint.north_bot[1];
				tint_north_bot[2] += _level->block(i - 1, j).lightingTint.north_bot[2];
			
				average_n[0] += 1;
			}
			if(i > 0 && isBlockBottomVisible(i - 1, j, DIR_S) &&
					fabs(_level->block(i - 1, j).height - block.height) < height_diff) {
				tint_south_bot[0] += _level->block(i - 1, j).lightingTint.south_bot[0];
				tint_south_bot[1] += _level->block(i - 1, j).lightingTint.south_bot[1];
				tint_south_bot[2] += _level->block(i - 1, j).lightingTint.south_bot[2];
				
				average_n[1] += 1;
			}
			if(i < _level->sizeX()-1 && isBlockBottomVisible(i + 1, j, DIR_N) &&
					fabs(_level->block(i + 1, j).height - block.height) < height_diff) {
				tint_north_bot[0] += _level->block(i + 1, j).lightingTint.north_bot[0];
				tint_north_bot[1] += _level->block(i + 1, j).lightingTint.north_bot[1];
				tint_north_bot[2] += _level->block(i + 1, j).lightingTint.north_bot[2];
			
				average_n[0] += 1;
			}
			if(i < _level->sizeX()-1 && isBlockBottomVisible(i + 1, j, DIR_S) &&
					fabs(_level->block(i + 1, j).height - block.height) < height_diff) {
				tint_south_bot[0] += _level->block(i + 1, j).lightingTint.south_bot[0];
				tint_south_bot[1] += _level->block(i + 1, j).lightingTint.south_bot[1];
				tint_south_bot[2] += _level->block(i + 1, j).lightingTint.south_bot[2];
				
				average_n[1] += 1;
			}
			
			if(i > 0 && isBlockTopVisible(i - 1, j, DIR_N) &&
					fabs(_level->block(i - 1, j).height - block.height) < height_diff) {
				const auto& b = _level->block(i - 1, j);
				const auto color_north = b.hasTop() ? b.lightingTint.north_top : b.lightingTint.north_bot;
			
				tint_north_top[0] += color_north[0];
				tint_north_top[1] += color_north[1];
				tint_north_top[2] += color_north[2];
			
				average_n[2] += 1;
			}
			if(i > 0 && isBlockTopVisible(i - 1, j, DIR_S) &&
					fabs(_level->block(i - 1, j).height - block.height) < height_diff) {
				const auto& b = _level->block(i - 1, j);
				const auto color_south = b.hasTop() ? b.lightingTint.south_top : b.lightingTint.south_bot;
				
				tint_south_top[0] += color_south[0];
				tint_south_top[1] += color_south[1];
				tint_south_top[2] += color_south[2];
				
				average_n[3] += 1;
			}
			if(i < _level->sizeX()-1 && isBlockTopVisible(i + 1, j, DIR_N) &&
					fabs(_level->block(i + 1, j).height - block.height) < height_diff) {
				const auto& b = _level->block(i + 1, j);
				const auto color_north = b.hasTop() ? b.lightingTint.north_top : b.lightingTint.north_bot;
			
				tint_north_top[0] += color_north[0];
				tint_north_top[1] += color_north[1];
				tint_north_top[2] += color_north[2];
			
				average_n[2] += 1;
			}	
			if(i < _level->sizeX()-1 && isBlockTopVisible(i + 1, j, DIR_S) &&
					fabs(_level->block(i + 1, j).height - block.height) < height_diff) {
				const auto& b = _level->block(i + 1, j);
				const auto color_south = b.hasTop() ? b.lightingTint.south_top : b.lightingTint.south_bot;
				tint_south_top[0] += color_south[0];
				tint_south_top[1] += color_south[1];
				tint_south_top[2] += color_south[2];
				
				average_n[3] += 1;
			}
			
			tint_north_bot[0] = tint_north_bot[0]/average_n[0];
			tint_north_bot[1] = tint_north_bot[1]/average_n[0];
			tint_north_bot[2] = tint_north_bot[2]/average_n[0];
			
			tint_south_bot[0] = tint_south_bot[0]/average_n[1];
			tint_south_bot[1] = tint_south_bot[1]/average_n[1];
			tint_south_bot[2] = tint_south_bot[2]/average_n[1];
			
			tint_north_top[0] = tint_north_top[0]/average_n[2];
			tint_north_top[1] = tint_north_top[1]/average_n[2];
			tint_north_top[2] = tint_north_top[2]/average_n[2];
			
			tint_south_top[0] = tint_south_top[0]/average_n[3];
			tint_south_top[1] = tint_south_top[1]/average_n[3];
			tint_south_top[2] = tint_south_top[2]/average_n[3];
			
			block.lightingTint.north_bot[0] = tint_north_bot[0];
			block.lightingTint.north_bot[1] = tint_north_bot[1];
			block.lightingTint.north_bot[2] = tint_north_bot[2];
			
			block.lightingTint.south_bot[0] = tint_south_bot[0];
			block.lightingTint.south_bot[1] = tint_south_bot[1];
			block.lightingTint.south_bot[2] = tint_south_bot[2];
			
			block.lightingTint.north_top[0] = tint_north_top[0];
			block.lightingTint.north_top[1] = tint_north_top[1];
			block.lightingTint.north_top[2] = tint_north_top[2];
			
			block.lightingTint.south_top[0] = tint_south_top[0];
			block.lightingTint.south_top[1] = tint_south_top[1];
			block.lightingTint.south_top[2] = tint_south_top[2];
		}
	}
}

bool LightingCalculator::isBlockBottomVisible(uint32_t x, uint32_t y, Dir dir) const
{
	const Block& block = _level->block(x, y); 
	
	bool vis_x0 = x > 0 &&
		_level->block(x - 1, y).standHeightVisual() < block.standHeightVisual();
	bool vis_x1 = x < _level->sizeX() - 1  &&
		_level->block(x + 1, y).standHeightVisual() < block.standHeightVisual();
	bool vis_y0 = y > 0 &&
		_level->block(x, y - 1).standHeightVisual() < block.standHeightVisual();
	bool vis_y1 = y < _level->sizeY() - 1 && 
		_level->block(x, y + 1).standHeightVisual() < block.standHeightVisual();
	
	return (dir == DIR_W && vis_x0) || (dir == DIR_E && vis_x1) ||
            (dir == DIR_S && vis_y0) || (dir == DIR_N && vis_y1);
}

bool LightingCalculator::isBlockTopVisible(uint32_t x, uint32_t y, Dir dir) const
{
	const Block& block = _level->block(x, y);
	if(!block.hasTop())
		return isBlockBottomVisible(x, y, dir);
	
	auto side_visible = [&block](const Block& b) {
		if(!b.hasTop() && b.height > block.height)
			return false;
		if(b.hasTop() &&
			b.notch + b.notchHeight < block.notch + block.notchHeight &&
			b.height > block.height)
			return false;
		if(b.hasTop() && b.notch > block.height)
			return false;
		
		return true;
	};
	
	bool vis_x0 = x > 0 && side_visible(_level->block(x - 1, y));
	bool vis_x1 = x < _level->sizeX() - 1 && side_visible(_level->block(x + 1, y));
	bool vis_y0 = y > 0 && side_visible(_level->block(x, y - 1));
	bool vis_y1 = y < _level->sizeY() - 1 && side_visible(_level->block(x, y + 1));
	
	return (dir == DIR_W && vis_x0) || (dir == DIR_E && vis_x1) ||
            (dir == DIR_S && vis_y0) || (dir == DIR_N && vis_y1);
}

void LightingCalculator::averageNorthSouth()
{
	for(size_t i = 0; i < _level->sizeX(); i++) {
		for(size_t j = 0; j < _level->sizeY(); j++) {
			auto& block = _level->block(i, j);
			
			uint32_t tint_west_bot[3];
			uint32_t tint_east_bot[3];
			uint32_t tint_west_top[3];
			uint32_t tint_east_top[3];
			
			tint_west_bot[0] = block.lightingTint.west_bot[0];
			tint_west_bot[1] = block.lightingTint.west_bot[1];
			tint_west_bot[2] = block.lightingTint.west_bot[2];
			
			tint_east_bot[0] = block.lightingTint.east_bot[0];
			tint_east_bot[1] = block.lightingTint.east_bot[1];
			tint_east_bot[2] = block.lightingTint.east_bot[2];
			
			tint_west_top[0] = block.lightingTint.west_top[0];
			tint_west_top[1] = block.lightingTint.west_top[1];
			tint_west_top[2] = block.lightingTint.west_top[2];
			
			tint_east_top[0] = block.lightingTint.east_top[0];
			tint_east_top[1] = block.lightingTint.east_top[1];
			tint_east_top[2] = block.lightingTint.east_top[2];
			
			uint8_t average_n[4] = {1, 1, 1, 1};
			
			const float height_diff = 3.0f;
			if(j > 0 && isBlockBottomVisible(i, j - 1, DIR_W) &&
					fabs(_level->block(i, j - 1).height - block.height) < height_diff) {
				tint_west_bot[0] += _level->block(i, j - 1).lightingTint.west_bot[0];
				tint_west_bot[1] += _level->block(i, j - 1).lightingTint.west_bot[1];
				tint_west_bot[2] += _level->block(i, j - 1).lightingTint.west_bot[2];
			
				average_n[0] += 1;
			}
			if(j > 0 && isBlockBottomVisible(i, j - 1, DIR_E) &&
					fabs(_level->block(i, j - 1).height - block.height) < height_diff) {
				tint_east_bot[0] += _level->block(i, j - 1).lightingTint.east_bot[0];
				tint_east_bot[1] += _level->block(i, j - 1).lightingTint.east_bot[1];
				tint_east_bot[2] += _level->block(i, j - 1).lightingTint.east_bot[2];
				
				average_n[1] += 1;
			}
			
			if(j < _level->sizeX()-1 && isBlockBottomVisible(i, j + 1, DIR_W) &&
				fabs(_level->block(i, j + 1).height - block.height) < height_diff) {
				tint_west_bot[0] += _level->block(i, j + 1).lightingTint.west_bot[0];
				tint_west_bot[1] += _level->block(i, j + 1).lightingTint.west_bot[1];
				tint_west_bot[2] += _level->block(i, j + 1).lightingTint.west_bot[2];
				
				average_n[0] += 1;
			}
			if(j < _level->sizeX()-1 && isBlockBottomVisible(i, j + 1, DIR_E) &&
				fabs(_level->block(i, j + 1).height - block.height) < height_diff) {
				tint_east_bot[0] += _level->block(i, j + 1).lightingTint.east_bot[0];
				tint_east_bot[1] += _level->block(i, j + 1).lightingTint.east_bot[1];
				tint_east_bot[2] += _level->block(i, j + 1).lightingTint.east_bot[2];
				
				average_n[1] += 1;
			}
			
			if(j > 0 && isBlockTopVisible(i, j - 1, DIR_W) &&
				fabs(_level->block(i, j - 1).height - block.height) < height_diff) { 
				const auto& b = _level->block(i, j - 1);
				const auto color_west = b.hasTop() ? b.lightingTint.west_top : b.lightingTint.west_bot;
			
				tint_west_top[0] += color_west[0];
				tint_west_top[1] += color_west[1];
				tint_west_top[2] += color_west[2];
				
				average_n[2] += 1;
			}
			if(j > 0 && isBlockTopVisible(i, j - 1, DIR_E) &&
				fabs(_level->block(i, j - 1).height - block.height) < height_diff) { 
				const auto& b = _level->block(i, j - 1);
				const auto color_east = b.hasTop() ? b.lightingTint.east_top : b.lightingTint.east_bot;
			
				tint_east_top[0] += color_east[0];
				tint_east_top[1] += color_east[1];
				tint_east_top[2] += color_east[2];
				
				average_n[3] += 1;
			}
			
			if(j < _level->sizeX()-1 && isBlockTopVisible(i, j + 1, DIR_W) &&
				fabs(_level->block(i, j + 1).height - block.height) < height_diff) {
				const auto& b = _level->block(i, j + 1);
				const auto color_west = b.hasTop() ? b.lightingTint.west_top : b.lightingTint.west_bot;
				tint_west_top[0] += color_west[0];
				tint_west_top[1] += color_west[1];
				tint_west_top[2] += color_west[2];
			
				average_n[2] += 1;
			}
			if(j < _level->sizeX()-1 && isBlockTopVisible(i, j + 1, DIR_E) &&
				fabs(_level->block(i, j + 1).height - block.height) < height_diff) {
				const auto& b = _level->block(i, j + 1);
				const auto color_east = b.hasTop() ? b.lightingTint.east_top : b.lightingTint.east_bot;
				tint_east_top[0] += color_east[0];
				tint_east_top[1] += color_east[1];
				tint_east_top[2] += color_east[2];
				
				average_n[3] += 1;
			}
			
			tint_west_bot[0] = tint_west_bot[0]/average_n[0];
			tint_west_bot[1] = tint_west_bot[1]/average_n[0];
			tint_west_bot[2] = tint_west_bot[2]/average_n[0];
			
			tint_east_bot[0] = tint_east_bot[0]/average_n[1];
			tint_east_bot[1] = tint_east_bot[1]/average_n[1];
			tint_east_bot[2] = tint_east_bot[2]/average_n[1];
			
			tint_west_top[0] = tint_west_top[0]/average_n[2];
			tint_west_top[1] = tint_west_top[1]/average_n[2];
			tint_west_top[2] = tint_west_top[2]/average_n[2];
			
			tint_east_top[0] = tint_east_top[0]/average_n[3];
			tint_east_top[1] = tint_east_top[1]/average_n[3];
			tint_east_top[2] = tint_east_top[2]/average_n[3];
			
			block.lightingTint.west_bot[0] = tint_west_bot[0];
			block.lightingTint.west_bot[1] = tint_west_bot[1];
			block.lightingTint.west_bot[2] = tint_west_bot[2];
			
			block.lightingTint.east_bot[0] = tint_east_bot[0];
			block.lightingTint.east_bot[1] = tint_east_bot[1];
			block.lightingTint.east_bot[2] = tint_east_bot[2];
			
			block.lightingTint.west_top[0] = tint_west_top[0];
			block.lightingTint.west_top[1] = tint_west_top[1];
			block.lightingTint.west_top[2] = tint_west_top[2];
			
			block.lightingTint.east_top[0] = tint_east_top[0];
			block.lightingTint.east_top[1] = tint_east_top[1];
			block.lightingTint.east_top[2] = tint_east_top[2];
		}
	}
}

void LightingCalculator::spritesPass()
{
	for(const auto& light : _level->lights()) {
		for(VisualEntity* e : _level->entities()) {
			if(e->lighting_static) {
				auto lp = vec3(light.x, light.y, light.z);
				auto ep = vec3(e->x(), e->y(), e->z() + e->visualHeight() / 2.0f);
				if(hitsLight(lp, ep)) {
					lightAdd(e->lighting_tint, light, (lp - ep).length());
				}
			}
		}
	}
}
