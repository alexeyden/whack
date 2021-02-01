#ifndef TEXTURESHEET_H
#define TEXTURESHEET_H

#include <vector>
#include <map>
#include <istream>
#include <algorithm>
#include <stdexcept>

#include "texture.h"
#include "ninepatch.h"
#include "animation.h"

class TextureAtlas
{
public:
	TextureAtlas(unsigned char*  data, unsigned width, unsigned height);
	~TextureAtlas();
	
	unsigned addTexture(unsigned x, unsigned y, unsigned w, unsigned h);
	unsigned addStatic(const std::string& name, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
	unsigned addNinePatch(const std::string& name, const NinePatch& ninepatch);
	void addAnimation(const std::string& name, AnimationType type, const Animation& anim);
	
	const std::vector<Texture>& textureList() const {
		return _textures;
	}
	
	const std::map<std::string, unsigned>& textureNameList() const {
		return _textureNames;
	}
	
	const std::map<std::string, std::map<AnimationType, Animation> >& animationList() const {
		return _animations;
	}
	
	const std::map<std::string, NinePatch>& ninePatchList() const {
		return _ninePatches;
	}
	
	const Texture& textureByName(const std::string& name) const {
		return _textures[textureID(name)];
	}
	
	const std::string& textureName(unsigned id) const;
	const std::string& animationName(const Animation& anim, AnimationType type) const;
	
	unsigned short textureID(const std::string& name) const {
		return _textureNames.at(name);
	}

	const Texture& texture(unsigned id) const {
		return _textures[id];
	}
	
	const NinePatch& ninePatch(const std::string& id) const {
		return _ninePatches.at(id);
	}
	
	const Animation& animation(const std::string& id, AnimationType type) const {
		return _animations.at(id).at(type);
	}
	
	const Texture& asTexture() const {
		return _textures[0];
	}
private:
	std::map<std::string, NinePatch> _ninePatches;
	std::vector<Texture> _textures;
	std::map<std::string, unsigned> _textureNames;
	std::map<std::string, std::map<AnimationType, Animation> > _animations;
};

#endif // TEXTURESHEET_H
