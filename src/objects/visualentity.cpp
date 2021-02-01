#include "visualentity.h"
#include "math.h"

AnimationViewDir VisualEntity::viewSide(float vx, float vy) const
{
	float dx = cos(_dir);
	float dy = sin(_dir);
	float dot = dx*vx + dy*vy;
	
	if(dot > 0.7) return AnimationViewDir::BACK;
	if(dot < -0.7) return AnimationViewDir::FRONT;
	
	if(fabs(dot) < 0.7 && -vx*dy + vy*dx >= 0)
		return AnimationViewDir::RIGHT;
	
	return AnimationViewDir::LEFT;
}
