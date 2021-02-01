#ifndef TEXTUREATLASREADER_H
#define TEXTUREATLASREADER_H

#include "textureatlas.h"

class TextureAtlasReader
{
public:
	enum AtlasFileType {
		AFT_IMAGE = 0,
		AFT_ATLAS = 1
	};

	TextureAtlasReader(const char* f, AtlasFileType type);
	~TextureAtlasReader();
	
	TextureAtlas* read();
	
private:
	TextureAtlas* readFromAtlas(const char* f);
	TextureAtlas* readFromImage(const char* fname); 
	
	TextureAtlas* atlas;
	
	const char* file;
	AtlasFileType fileType;
};

#endif // TEXTUREATLASREADER_H
