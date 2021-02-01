#ifndef RESOURCES_H
#define RESOURCES_H

#include <map>
#include <string>

#include "graphics/textureatlas.h"
#include "backend/backend.h"

class Resources
{
private:
	Resources();
	
public:
	~Resources();

	void loadAtlas(const char* path, const std::string& id);
	
	const TextureAtlas* atlas(const std::string& id) const {
		return _sheets.at(id);
	}
	
	const std::map<std::string, TextureAtlas*>& atlases() const {
		return _sheets;
	}
	
	static void init(Backend* be);
	static Resources* i();
    
private:
	std::map<std::string, TextureAtlas*> _sheets;
	
    static Backend* _backend;
	static Resources* _instance;
};

#endif // RESOURCES_H

