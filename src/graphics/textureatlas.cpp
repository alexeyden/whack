#include "textureatlas.h"

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <fstream>

#include "game/application.h"
#include "util/util.h"

TextureAtlas::TextureAtlas(unsigned char* data, unsigned int width, unsigned int height)
{
	_textures.push_back(Texture(data, 0, 0, width, height, width, height));
}

TextureAtlas::~TextureAtlas()
{
	delete _textures[0].data();
}

unsigned int TextureAtlas::addStatic(const std::string& name, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	_textures.push_back(Texture(_textures[0].data(), x, y, w, h, _textures[0].width(), _textures[0].height()));
	_textureNames[name] = _textures.size() - 1;
	
	return _textures.size() - 1;
}

unsigned int TextureAtlas::addTexture(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	_textures.push_back(Texture(_textures[0].data(), x, y, w, h, _textures[0].width(), _textures[0].height()));
	
	return _textures.size() - 1;
}

unsigned int TextureAtlas::addNinePatch(const std::string& name, const NinePatch& ninepatch)
{
	_ninePatches.insert(std::pair<std::string, NinePatch>(name, ninepatch));
    return _ninePatches.size();
}

void TextureAtlas::addAnimation(const std::string& name, AnimationType type, const Animation& anim)
{
	_animations[name][type] = anim;
}

const std::string& TextureAtlas::textureName(unsigned int id) const
{
	auto pred = [&id](const std::pair<std::string, unsigned>& x) {
		return x.second == id;
	};
	auto iter = std::find_if(_textureNames.begin(), _textureNames.end(), pred);
	
	if(iter == _textureNames.end())
		throw std::out_of_range("no item with such id");
	
	return iter->first;
}

const std::string& TextureAtlas::animationName(const Animation& anim, AnimationType type) const
{
	auto pred = [&anim,&type](const std::pair<std::string, std::map<AnimationType, Animation>>& x) -> bool {
			return x.second.find(type)->second == anim;
	};
	
	auto iter = std::find_if(_animations.begin(), _animations.end(), pred);
	if(iter == _animations.end())
		throw std::out_of_range("no such animation");
	
	return iter->first;
}


