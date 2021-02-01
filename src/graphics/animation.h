#ifndef ANIMATION_H
#define ANIMATION_H

#include <cstring>
#include <algorithm>
#include <cstdint>

enum AnimationType {
	IDLE = 0,
	WALK = 1,
	ATTACK = 2,
	DEATH = 3
};

enum AnimationViewDir {
	FRONT = 0,
	BACK = 1,
	LEFT = 2,
	RIGHT = 3
};

class Animation
{
public:
	Animation(unsigned startFrame, unsigned endFrame)
		: directional(false)
	{
		this->startFrame[0] = startFrame;
		this->endFrame[0] = endFrame;
	}
	
	Animation() : directional(true)
	{
		memset(startFrame, 0, sizeof(startFrame));
		memset(endFrame, 0, sizeof(endFrame));
	}
	
	bool operator==(const Animation& anim) const {
		if(directional != anim.directional)
			return false;
		
		if(directional)
			return std::mismatch(startFrame, startFrame + 6, anim.startFrame).first == startFrame + 6 &&
				std::mismatch(endFrame, endFrame + 6, anim.endFrame).first == endFrame + 6;
		
		return startFrame[0] == anim.startFrame[0] && endFrame[0] == anim.endFrame[0];
	}
	
	uint32_t startFrame[4];
	uint32_t endFrame[4];
	
	bool directional;
};

#endif // ANIMATION_H
