#include "resources.h"

#include <cstdlib>
#include <exception>
#include <cstring>

#include "graphics/textureatlasreader.h"

Resources* Resources::_instance = nullptr;
Backend* Resources::_backend = nullptr;

Resources::Resources()
{

}

Resources::~Resources()
{
	for(auto it = _sheets.begin(); it != _sheets.end(); it++)
		delete it->second;
	
	_sheets.clear();
}

Resources* Resources::i()
{
	if(_instance == nullptr)
		_instance = new Resources();
	
	return _instance;
}

void Resources::init(Backend* be) {
    _backend = be;
}

void Resources::loadAtlas(const char* path, const std::string& id)
{
	TextureAtlas* a = nullptr;
	const char ext[] = ".json"; 
     
	TextureAtlasReader::AtlasFileType type = TextureAtlasReader::AFT_IMAGE;
	
	if(strlen(path) > strlen(ext) && strcmp(path + strlen(path) - strlen(ext), ext) == 0) {
		type = TextureAtlasReader::TextureAtlasReader::AFT_ATLAS;
    }
    
	TextureAtlasReader reader(path, type);
	a = reader.read();
    
	if(a == nullptr) //TODO: error handling
		return;

	_sheets[id] = a;
}
