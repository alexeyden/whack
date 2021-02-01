#ifndef RENDERER_H
#define RENDERER_H

#include <cmath>
#include <list>
#include "level.h"
#include "graphics/font.h"
#include "graphics/texture.h"
#include "util/rangeset.h"
#include "util/raytracer.h"

class TextureAtlas;

class Renderer
{
public:
	Renderer(Level* map, const TextureAtlas* atlas, unsigned width, unsigned height); 
	~Renderer();

	int viewWidth;
	int viewHeight;
	
	double camX, camY, camZ;
	double camRot;
	
	double zNear, zFar;
	
	void render(Texture* out); 
	void update();
	
	double getFOV() const {
		return atanf(0.66 * 2.0);
	}
	
	void path(int x, int y);
	
	std::list<vec3> points;
	
	bool drawEntityCells;
private:
	Font* _dbgFont;
	
	Level* map;
	RayTracer* tracer;
	
	const TextureAtlas* _atlas;
	
	double* zBuffer;
	double camDirX, camDirY;
	double planeX, planeY;

	double project(double h, double dist) {
		return 0.5 - (camZ - h + 0.5)/dist;
	}
	
	void mix(const unsigned char* src, unsigned char* dst, double distance);
	
	int clamp(int y) {
		return std::min<int>(viewHeight - 1, std::max<int>(0, y));
	}
	
	void renderSprites(const std::set<Entity*>& visibleEnts, Texture* out);
	void renderSprite(const VisualEntity* sp, Texture* out);
	//void renderDecals(Texture* out);
	
	void renderWallPart(int x, const std::vector<Range<int> >& parts,
											int yStart, double heightWorld, int heightScreen, bool isTop,
											float rayDirX, float rayDirY, const TraceInfo& info, Texture* out); 
	void renderFloorCeilingPart(int x, const std::vector<Range<int> >& parts,
											 double height, bool isCeiling, const TraceInfo& info, Texture* out);
	void renderEdgeFloorPart(int x, const std::vector<Range<int>>& parts, double height, const TraceInfo& info, Texture* out);
	
	void renderSky(int x, const std::vector<Range<int> >& parts, float angle, Texture* out);
	
	void renderLine(const vec3& v0, const vec3& v1, unsigned colorBGR, Texture* out);
	void renderPolyLine(std::list<vec3> points,
											unsigned colorBGR,
											Texture* out);
};

#endif // RENDERER_H
