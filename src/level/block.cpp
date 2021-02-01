#include "block.h"

#include <algorithm>

#include "util/util.h"

BlockAnimationManager Block::animationManager;

Block::Block()
	:  height(1.0f), notch(0.0f), notchHeight(0.0f),  liquid(0), tracerVisit(0), animationBits(0)
{
	std::fill(textures, textures + 6, 0);
}

Block::Block(float height, unsigned int tex) 
	: height(height), notch(0.0f), notchHeight(0.0f), liquid(0), 
	tracerVisit(0), animationBits(0) 
{
	std::fill(textures, textures + 6, tex);
}

unsigned Block::texture(BlockTex side) const
{
	if(animationBits & 1 << side)
		return animationManager.frame(textures[side]);
	
	return textures[side];
}

bool Block::isAnimated(BlockTex side) const
{
	return animationBits & 1 << side;
}

const Animation* Block::animation(BlockTex side) const
{
	if(animationBits & 1 << side)
		return &animationManager.animation(textures[side]);
	
	return nullptr;
}

void Block::texture(BlockTex side, const Animation& anim)
{
	textures[side] = animationManager.add(anim);
	animationBits |= 1 << side;
}

void Block::texture(BlockTex side, unsigned int staticId)
{
	textures[side] = staticId;
	animationBits &= ~(1 << side);
}

void Block::update(double dt)
{
	Block::animationManager.update(dt);
}

bool Block::operator==(const Block& op) const {
	for(int i = 0; i < 6; i++)
		if(textures[i] != op.textures[i])
			return false;
		
	if(animationBits != op.animationBits)
		return false;
	
	if(height != op.height || notch != op.notch || notchHeight != op.notchHeight)
		return false;
	
	if(liquid != op.liquid)
		return false;
	
	return true;
}


BlockAnimationManager::BlockAnimationManager()
	: _time(0.0) {}

unsigned int BlockAnimationManager::add(const Animation& anim)
{
	auto iter = std::find_if(
		_frames.begin(), _frames.end(),
		[&anim](const std::pair<unsigned, Animation>& x) -> bool { return x.second == anim; }
	);
	
	if(iter != _frames.end())
		return iter - _frames.begin();
	
	_frames.push_back(std::pair<unsigned, Animation>(anim.startFrame[0], anim));
	
	return _frames.size() - 1;
}

unsigned int BlockAnimationManager::frame(unsigned int id) const
{
	return _frames[id].first;
}

const Animation& BlockAnimationManager::animation(unsigned int id) const
{
	return _frames[id].second;
}

void BlockAnimationManager::update(double dt)
{
	_time += dt;
	
	if(_time >= _frameTime) {
		_time -= _frameTime;
		
		for(auto& frame_anim : _frames) {
			if(++frame_anim.first > frame_anim.second.endFrame[0]) {
				frame_anim.first = frame_anim.second.startFrame[0];
			}
		}
	}
}
