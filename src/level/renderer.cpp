#include "renderer.h"

#include <cstdlib>
#include <algorithm>
#include <cfloat>
#include <cstdint>
#include <cmath>
#include <vector>
#include <set>
#include <cstdio>
#include <cstring>
#include <string>
#include <climits>
#include <cassert>

#include "game/application.h"
#include "graphics/textureatlas.h"
#include "graphics/renderer2d.h"
#include "objects/visualentity.h"
#include "objects/decoration.h"
#include "level/pathfinder.h"
#include "util/rangeset.h"
#include "util/math.h"
#include "util/util.h"

Renderer::Renderer (Level* map, const TextureAtlas* atlas, unsigned int width, unsigned int height )
{
	this->map = map;
	
	viewWidth = width;
	viewHeight = height; 
	
	_atlas = atlas;
	
	camX = 1.5;
	camY = 1.5;
	camZ = 0.5;
	camRot = 0.0;
	update();
	
	zFar = 100;
	zNear = 1;
	
	zBuffer = new double[viewWidth * viewHeight];
	std::fill(zBuffer, zBuffer + viewWidth * viewHeight, FLT_MAX);
	
	tracer = new RayTracer(map, new EmptyEntityFilter(), 32, 100.0f);
	
	_dbgFont = new Font(&Resources::i()->atlas("font")->textureList()[0], 8);
	
	drawEntityCells = false;
}

void Renderer::path(int x, int y)
{
	PathFinder path(map);
	auto list = path.findPath(point2i(0, 0), point2i(x,y));
	points.clear();
	for(auto point: list) {
		points.push_back(vec3(point.x + 0.5f, point.y + 0.5f, map->block(point.x, point.y).standHeight() + 0.5f));
	}
}

Renderer::~Renderer()
{
	delete [] zBuffer;
	delete tracer;
}

void Renderer::update()
{
	camRot = map->player()->dir();
	camX = map->player()->x();
	camY = map->player()->y();
	camZ = map->player()->viewZ();
	
	camDirX = cos(camRot);
	camDirY = sin(camRot);
	planeX = 0.66 * cos(camRot - M_PI/2);
	planeY = 0.66 * sin(camRot - M_PI/2);
}

void Renderer::mix(const unsigned char* src, unsigned char* dst, double distance)
{
	float k = (distance < zNear) ? 1.0 : (zFar - std::min(distance, zFar))/(zFar - zNear);
	
	dst[0] = src[0] * k + dst[0] * (1.0f - k);
	dst[1] = src[1] * k + dst[1] * (1.0f - k);
	dst[2] = src[2] * k + dst[2] * (1.0f - k);
}

void Renderer::render (Texture* out )
{
	for(int i = 0; i < viewWidth * viewHeight; i++)
		zBuffer[i] = FLT_MAX;
	
	std::vector<Range<int>> parts; //segments to draw
	RangeSet<int> drawn(32); //drawn segments
	
	tracer->reset();
	for(int x = 0; x < viewWidth; x++) {
		double planePos = 2 * x/double(viewWidth) - 1; 
		double rayDirX = camDirX + planeX * planePos;
		double rayDirY = camDirY + planeY * planePos;
		double rayAngle = atan2(rayDirY, rayDirX);

		const std::vector<TraceInfo>& hits = tracer->trace(camX, camY, rayDirX, rayDirY, true);
		
		drawn.clear();
		parts.clear();
		
		for(size_t i = 0; i < hits.size(); i++) {
			const TraceInfo& ti = hits[i];
			const Block& block = map->block(ti.start.x, ti.start.y);
			
			int yBottom, yTop, yFloor;
			
			if(block.notchHeight > 0) { //composite block
				//render wall bottom part
				yBottom = ceil(viewHeight * project(0, ti.start.distance));
				yTop = floor(viewHeight * project(block.notch, ti.start.distance));
                
				parts = drawn.diff(Range<int>(clamp(yBottom), clamp(yTop)));
				drawn.addRange(clamp(yBottom), clamp(yTop));
				
				renderWallPart(x, parts, yBottom, block.notch, abs(yTop-yBottom), false, rayDirX, rayDirY, ti, out);
				
				//render wall top part
				yBottom = floor(viewHeight * project(block.notch + block.notchHeight, ti.start.distance));
				yTop = ceil(viewHeight * project(block.height, ti.start.distance));

				if(yBottom < viewHeight) {
					parts = drawn.diff(Range<int>(clamp(yBottom), clamp(yTop)));
					drawn.addRange(clamp(yBottom), clamp(yTop));
					
					renderWallPart(x, parts, yBottom, block.height - block.notchHeight - block.notch,
												abs(yTop-yBottom), true, rayDirX, rayDirY, ti, out);
				}
			}
			else { //simple block
				//render wall
				yBottom = ceil(viewHeight * project(0, ti.start.distance));
				yTop = floor(viewHeight * project(block.height, ti.start.distance));
			
				parts = drawn.diff(Range<int>(clamp(yBottom), clamp(yTop)));
				drawn.addRange(clamp(yBottom), clamp(yTop));
					
				renderWallPart(x, parts, yBottom, block.height, abs(yTop-yBottom), false, rayDirX, rayDirY, ti, out);
			}
			
			if(block.notchHeight > 0) { //composite block
				//bottom part floor
				yTop = ceil(viewHeight * project(block.notch, ti.start.distance)); 
				yFloor = floor(viewHeight * project(block.notch, ti.end.distance)); 
                
				parts = drawn.diff(Range<int>(clamp(yTop), clamp(yFloor)));
				drawn.addRange(clamp(yTop), clamp(yFloor));
				renderFloorCeilingPart(x,  parts, block.notch, false, ti, out);
				
				//top part ceiling
				yTop = ceil(viewHeight * project(block.notch + block.notchHeight, ti.end.distance)); 
				yFloor = floor(viewHeight * project(block.notch + block.notchHeight, ti.start.distance));
				
				if(yTop < viewHeight) {
					parts = drawn.diff(Range<int>(clamp(yTop), clamp(yFloor)));
					drawn.addRange(clamp(yTop), clamp(yFloor));
					renderFloorCeilingPart(x,  parts, block.notch + block.notchHeight, true, ti, out);
				}
			}
			else { //simple block floor
				if(block.height < (camZ + 0.5)) { //render only if top edge is below view point
					yFloor = floor(viewHeight * project(block.height, ti.end.distance));
			
					parts = drawn.diff(Range<int>(clamp(yTop), clamp(yFloor)));
					drawn.addRange(clamp(yTop), clamp(yFloor));
					renderFloorCeilingPart(x,  parts, block.height, false, ti, out);
				}
			}
		}
		
		if(map->clampEdges) {
			parts = drawn.diff(Range<int>(0, viewHeight/2));
			drawn.addRange(0, viewHeight/2); 
			
			TraceHit end;
			end.distance = 10000.0f;
			end.x = rayDirX * end.distance; 
			end.y = rayDirY * end.distance;
			end.hitSide = DIR_N;
			end.wallDisp = 0;
			
			TraceInfo ti;
			ti.hitBorder = false;
			ti.start = hits[hits.size()-1].end;
			ti.end = end;
            
			renderEdgeFloorPart(x, parts, map->block(0, 0).height, ti, out);
		}
		
		//render sky
		parts = drawn.diff(Range<int>(0, viewHeight-1));
		renderSky(x, parts, rayAngle, out);
	}
	
	renderSprites(tracer->hitEntities(), out);
	
	/*
	std::list<vec3> points {
		vec3(1, 1, 2),
		vec3(4, 4, 2),
		vec3(2, 3, 3),
		vec3(2, 3, 4),
		vec3(3, 3, 2),
		vec3(1, 1, 2)
	};
	*/
	renderPolyLine(points, colorBGR(0x80, 0x00, 0x80), out);
	//renderLine(vec3(0, 0, 1), vec3(3, 3, 3), 0x00ffffff, out);
}

void Renderer::renderWallPart(int x, const std::vector< Range< int > >& parts,
															int yStart, double heightWorld, int heightScreen, bool isTop,
															float rayDirX, float rayDirY, const TraceInfo& info, Texture* out)
{
	const Block& block = map->block(info.start.x, info.start.y);
	
	int texX = int(info.start.wallDisp * 32);
	
	if(((info.start.hitSide == DIR_E || info.start.hitSide == DIR_W) && rayDirX > 0) ||
		((info.start.hitSide == DIR_N || info.start.hitSide == DIR_S) && rayDirY < 0))
			texX = 32 - texX - 1;

	const auto& decals = block.decals(BlockTex(info.start.hitSide + 2));
	
    uint8_t resultColor[3];
    uint8_t textureColor[3];
    
	for(const Range<int>& part : parts) {
		for(int y = part.start; y <= part.end; y++)
		{
			if(zBuffer[y * viewWidth + x] > info.start.distance)
				zBuffer[y * viewWidth + x] = info.start.distance;
		
			double wallY = double((y - yStart) * heightWorld) / double(heightScreen);
			int texY = 31 - int(wallY * 32) % 32;
			int texID = block.texture(BlockTex(info.start.hitSide + 2));
			
			const unsigned char* color = _atlas->textureList()[texID].pixelPtr(texX, texY);
            std::copy(color, color + 3, textureColor);
			
			const uint32_t *tint = nullptr;
			
			if(!isTop) {
				tint = block.lightingTint.north_bot;
				if(info.start.hitSide == DIR_S) tint = block.lightingTint.south_bot;
				if(info.start.hitSide == DIR_W) tint = block.lightingTint.west_bot;
				if(info.start.hitSide == DIR_E) tint = block.lightingTint.east_bot;
			}
			else {
				tint = block.lightingTint.north_top;
				if(info.start.hitSide == DIR_S) tint = block.lightingTint.south_top;
				if(info.start.hitSide == DIR_W) tint = block.lightingTint.west_top;
				if(info.start.hitSide == DIR_E) tint = block.lightingTint.east_top;
			}
			
			const float offsetX = info.start.wallDisp;
			const float offsetY = wallY; 
            
			for(const auto& decal : decals) {
				if(offsetX >= decal.offsetX && offsetY >= decal.offsetY &&
					offsetX < decal.offsetX + decal.sizeX && offsetY < decal.offsetY + decal.sizeY) {
					const auto& decalTex = _atlas->textureList()[decal.texture];
					const uint_fast32_t decalX = (offsetX - decal.offsetX)/decal.sizeX * decalTex.width(); 
					const uint_fast32_t decalY = (offsetY - decal.offsetY)/decal.sizeY * decalTex.height();
					const unsigned char* colorDecal = decalTex.pixelPtr(decalX, decalY);
				
					if(colorDecal[2] == Texture::keyColorR &&
						colorDecal[1] == Texture::keyColorG &&
						colorDecal[0] == Texture::keyColorB) {
							continue;
					}
					
					std::copy(colorDecal, colorDecal + 3, textureColor);
				}
			}
		
            textureColor[0] = (uint8_t)std::min<uint32_t>(uint32_t(textureColor[0]) * tint[2] >> 8u, 0xff);
            textureColor[1] = (uint8_t)std::min<uint32_t>(uint32_t(textureColor[1]) * tint[1] >> 8u, 0xff);
            textureColor[2] = (uint8_t)std::min<uint32_t>(uint32_t(textureColor[2]) * tint[0] >> 8u, 0xff);
			
			map->sky()->color((y << 8)/out->width(), resultColor);
			
			mix(textureColor, resultColor, info.start.distance);
			
			out->setPixelFastInvY(x, y, resultColor[2], resultColor[1], resultColor[0]); 
		}
	}
}

void Renderer::renderFloorCeilingPart(int x, const std::vector< Range< int > >& parts, double height, bool isCeiling,
															 const TraceInfo& info, Texture* out)
{
	double floorWallX, floorWallY;
	
	if(info.end.hitSide == DIR_W) {
		floorWallX = info.end.x;
		floorWallY = info.end.y + info.end.wallDisp;
	}
	else if(info.end.hitSide == DIR_E) {
		floorWallX = info.end.x + 1.0;
		floorWallY = info.end.y + info.end.wallDisp;
	}
	else if(info.end.hitSide == DIR_S) {
		floorWallX = info.end.x + info.end.wallDisp;
		floorWallY = info.end.y;
	}
	else {
		floorWallX = info.end.x + info.end.wallDisp;
		floorWallY = info.end.y + 1.0;
	}
	
	float blockDist = info.end.distance;

	for(const Range<int>& r : parts) {
		for(int y = r.start; y <= r.end; y++) {
			double dist = 1.0 / (1.0 - (double(y)/viewHeight + (camZ - height))/(0.5 + (camZ - height)));
			if(dist == 1.0/0.0)
				continue;
			double k = dist/blockDist;
			double floorX = fabs(k * floorWallX + (1.0 - k) * camX);
			double floorY = fabs(k * floorWallY + (1.0 - k) * camY);
			
			if(zBuffer[y * viewWidth + x] > dist)
				zBuffer[y * viewWidth + x] = dist;
				
			int floorTexX = int(floorX * 32) % 32;
			int floorTexY = int(floorY * 32) % 32;
		
			
			const Block& block = map->block(int(floorX), int(floorY));
			int texID = isCeiling ? block.texture(BT_CEILING) : block.texture(BT_FLOOR);
			
			const uint8_t* color = _atlas->textureList()[texID].pixelPtr(floorTexX, floorTexY);
			
			unsigned char color_gray[3] = {0xf0, 0x00, 0x00};
			unsigned char color_black[3] = {0x0, 0x0, 0xff};
			
			if(drawEntityCells) {
				Rect<float> block_r(floor(floorX), floor(floorY), floor(floorX) + 1, floor(floorY) + 1);
				bool has_entity = map->entitiesAt(floorX, floorY).size() > 0;
				if(has_entity && (floorTexX % 2 != floorTexY % 2)) {
					bool entity_bbox = false;
					for(const auto& e : map->entitiesAt(floorX, floorY)) {
						Rect<float> e_r(e->x() - e->width()/2.0f, e->y() - e->height()/2.0f, e->x() + e->width()/2.0f, e->y() + e->height()/2.0f);
						if(block_r.intersects(e_r))
							entity_bbox = true;
					}
					
					color = entity_bbox ? color_black : color_gray;
				}
			}
			
			uint8_t textureColor[3] = {
				color[0], color[1], color[2]
			};
			
			const auto& decals = block.decals(BlockTex(isCeiling ? BT_CEILING : BT_FLOOR));
			const float offsetX = floorX - floor(floorX);
			const float offsetY = floorY - floor(floorY);
			
			for(const auto& decal : decals) {
				if(offsetX >= decal.offsetX && offsetY >= decal.offsetY &&
					offsetX < decal.offsetX + decal.sizeX && offsetY < decal.offsetY + decal.sizeY)
				{
					const auto& decalTex = _atlas->textureList()[decal.texture];
					const uint_fast32_t decalX = (offsetX - decal.offsetX)/decal.sizeX * decalTex.width(); 
					const uint_fast32_t decalY = (offsetY - decal.offsetY)/decal.sizeY * decalTex.height();
					const unsigned char* colorDecal = decalTex.pixelPtr(decalX, decalY);
				
					if(colorDecal[2] == Texture::keyColorR &&
						colorDecal[1] == Texture::keyColorG &&
						colorDecal[0] == Texture::keyColorB) {
							continue;
					}
					
					if(decal.blend) {
						textureColor[0] =
							0xff - ((0xffu - static_cast<uint_fast32_t>(textureColor[0])) * (0xffu - static_cast<uint_fast32_t>(colorDecal[0])) >> 8);
						textureColor[1] =
							0xff - ((0xffu - static_cast<uint_fast32_t>(textureColor[1])) * (0xffu - static_cast<uint_fast32_t>(colorDecal[1])) >> 8);
						textureColor[2] =
							0xff - ((0xffu - static_cast<uint_fast32_t>(textureColor[2])) * (0xffu - static_cast<uint_fast32_t>(colorDecal[2])) >> 8);
					} else {
						std::copy(colorDecal, colorDecal + 3, textureColor);
					}
				}
			}
			
			const uint32_t *tint = isCeiling ? block.lightingTint.ceiling : block.lightingTint.floor;
			textureColor[0] = (uint8_t) std::min<uint32_t>(uint32_t(textureColor[0]) * tint[2] >> 8u, 0xff);
			textureColor[1] = (uint8_t) std::min<uint32_t>(uint32_t(textureColor[1]) * tint[1] >> 8u, 0xff);
			textureColor[2] = (uint8_t) std::min<uint32_t>(uint32_t(textureColor[2]) * tint[0] >> 8u, 0xff);
			
			unsigned char colorSky[3];
			map->sky()->color((y << 8)/out->width(), colorSky);
			mix(textureColor, colorSky, dist);
			
			out->setPixelFastInvY(x, y, colorSky[2], colorSky[1], colorSky[0]); 
		}
	}
}

void Renderer::renderEdgeFloorPart(int x, const std::vector< Range< int > >& parts, double height, const TraceInfo& info, Texture* out)
{
	double floorWallX, floorWallY;
	
	if(info.end.hitSide == DIR_W) {
		floorWallX = info.end.x;
		floorWallY = info.end.y + info.end.wallDisp;
	}
	else if(info.end.hitSide == DIR_E) {
		floorWallX = info.end.x + 1.0;
		floorWallY = info.end.y + info.end.wallDisp;
	}
	else if(info.end.hitSide == DIR_S) {
		floorWallX = info.end.x + info.end.wallDisp;
		floorWallY = info.end.y;
	}
	else {
		floorWallX = info.end.x + info.end.wallDisp;
		floorWallY = info.end.y + 1.0;
	}
	
	float blockDist = info.end.distance;
    
    unsigned char targetColor[3];
    unsigned char colorSky[3];

	for(const Range<int>& r : parts) {
		for(int y = r.start; y <= r.end; y++) {
			double dist = 1.0 / (1.0 - (double(y)/viewHeight + (camZ - height))/(0.5 + (camZ - height)));
			double k = dist/blockDist;
			double floorX = fabs(k * floorWallX + (1.0 - k) * camX);
			double floorY = fabs(k * floorWallY + (1.0 - k) * camY);
			
			if(zBuffer[y * viewWidth + x] > dist)
				zBuffer[y * viewWidth + x] = dist;
				
			int floorTexX = int(floorX * 32) % 32;
			int floorTexY = int(floorY * 32) % 32;
		
			const Block& block = map->block(0,0);
			int texID = block.texture(BT_FLOOR);
			
			const unsigned char* color = _atlas->textureList()[texID].pixelPtr(floorTexX, floorTexY);
            
			const uint32_t *tint = block.lightingTint.floor;
			targetColor[0] = (uint8_t) std::min<uint32_t>(uint32_t(color[0]) * tint[2] >> 8u, 0xff);
			targetColor[1] = (uint8_t) std::min<uint32_t>(uint32_t(color[1]) * tint[1] >> 8u, 0xff);
			targetColor[2] = (uint8_t) std::min<uint32_t>(uint32_t(color[2]) * tint[0] >> 8u, 0xff);
			
			map->sky()->color((y << 8)/out->width(), colorSky);
			mix(targetColor, colorSky, dist);
			
			out->setPixelFastInvY(x, y, colorSky[2], colorSky[1], colorSky[0]); 
		}
	}
}


void Renderer::renderSky(int x, const std::vector< Range< int > >& parts, float angle, Texture* out)
{
	if(map->sky() != nullptr) {
		map->sky()->render(x, parts, angle, out);
	}
}

void Renderer::renderSprite(const VisualEntity* e, Texture* out)
{
	double spX = e->x() - camX;
	double spY = e->y() - camY;
	double spZ = e->z();
	
	double invDet = 1.0 / (planeX * camDirY - camDirX * planeY);
	double transfX = invDet * (camDirY * spX - camDirX * spY);
	double transfY = invDet * (-planeY * spX + planeX * spY);	
	
	int spScreenX = int((viewWidth/2) * (1 + transfX/transfY));
	int spWidth = abs((int(viewHeight * e->visualWidth() / transfY)));
	int spScreenY0 = viewHeight * project(spZ, transfY);
	int spScreenY1 = viewHeight * project(spZ + e->visualHeight(), transfY);
	int spHeight = abs(spScreenY1 - spScreenY0) + 1;
	
	const Texture& tex = e->frame(camDirX, camDirY);
	const Block& block = map->block(e->x(), e->y());
	
	for(int x = std::max(spScreenX - spWidth/2, 0); x < std::min(spScreenX + spWidth/2, viewWidth); x++) {
		int texX = int(256 * (x - (-spWidth/2 + spScreenX)) * tex.width() / spWidth)/256;
		
		if(transfY > 0.5f && x > 0 && x < viewWidth)
		for(int y = std::max(spScreenY0, 0); y <  std::min(spScreenY1 + 1, viewHeight); y++) {
			int texY = (spScreenY1 - y) * tex.height() / spHeight; 
		
			const unsigned char* color = tex.pixelPtr(texX, texY);
			if(color[2] == Texture::keyColorR &&
				 color[1] == Texture::keyColorG &&
				 color[0] == Texture::keyColorB) {
				continue;
			}
			
			const uint32_t *tint = e->lighting_static ? e->lighting_tint : block.lightingTint.floor;
            
			uint8_t textureColor[3] = {
				(uint8_t) std::min<uint32_t>(uint32_t(color[0]) * tint[2] >> 8u, 0xff),
				(uint8_t) std::min<uint32_t>(uint32_t(color[1]) * tint[1] >> 8u, 0xff),
				(uint8_t) std::min<uint32_t>(uint32_t(color[2]) * tint[0] >> 8u, 0xff)
			};
			
			if(transfY >= zBuffer[y * viewWidth + x])
				continue;
			
			zBuffer[y * viewWidth + x] = transfY; 
			
			unsigned char colorSky[3];
			map->sky()->color((y << 8)/out->width(), colorSky);
			mix(textureColor, colorSky, transfY);
			
			out->setPixelFastInvY(x, y, colorSky[2], colorSky[1], colorSky[0]);
		}
	}
}

void Renderer::renderSprites(const std::set<Entity*>& visibleEnts, Texture* out)
{
	int dbg_rendered = 0;
	
	for(auto e : visibleEnts) {
		dbg_rendered ++;
		renderSprite((VisualEntity*) e, out);
	}
	
	char bf[32];
	sprintf(bf, "sprites: %d", dbg_rendered);
	_dbgFont->drawText(out, 2, 8, bf);
}

void Renderer::renderLine(const vec3& v0, const vec3& v1, unsigned int colorBGR, Texture* out)
{
	const double invDet = 1.0 / (planeX * camDirY - camDirX * planeY);
	
	//camera space
	const double tx0 = invDet * (camDirY * (v0.x - camX) - camDirX * (v0.y - camY));
	double ty0 = invDet * (-planeY * (v0.x - camX) + planeX  * (v0.y - camY));
	const double tx1 = invDet * (camDirY * (v1.x - camX) - camDirX * (v1.y - camY));
	double ty1 = invDet * (-planeY * (v1.x - camX) + planeX  * (v1.y - camY));
	
	//screen space
	int sx0 = int((viewWidth/2.0) * (1.0 + tx0/ty0));
	int sy0 = viewHeight * project(v0.z, ty0);
	int sx1 = int((viewWidth/2.0) * (1.0 + tx1/ty1));
	int sy1 = viewHeight * project(v1.z, ty1);
	
	if(sx0 >=0 && sx0 < viewWidth && sy0 >= 0 && sy0 < viewHeight)
	out->setPixelFastInvY(sx0, sy0, 0xff, 0x00, 0x00); 
	if(sx1 >=0 && sx1 < viewWidth && sy1 >= 0 && sy1 < viewHeight)
	out->setPixelFastInvY(sx1, sy1, 0xff, 0x00, 0x00); 
	
	const bool steep = fabs(sy1 - sy0) > fabs(sx1 - sx0);
	
	if(steep) {
		std::swap(sx0, sy0);
		std::swap(sx1, sy1);
	}
	
	if(sx0 > sx1) {
		std::swap(sx0, sx1);
		std::swap(sy0, sy1);
		std::swap(ty0, ty1);
	}
	
	float z0 = ty0;
	float z1 = ty1;
	
	const float dx = sx1 - sx0;
	const float dy = fabs(sy1 - sy0);
	
	float err = dx / 2.0f;
	
	const int ystep = (sy0 < sy1) ? 1 : -1;
	
	int y = sy0;
	
	int xmin = sx0;
	int xmax = sx1;
	const int xlimit = steep ? viewHeight : viewWidth;

	if(xmin < 0) {
		float t = 1.0f - float(xmax - 0)/float(xmax - xmin);
		z0 = 1.0f/(1.0f/ty1 * t + 1.0f/ty0 * (1.0f - t));
	}
	if(xmax > xlimit) {
		float t = 1.0f - float(xmax - xlimit)/float(xmax - xmin);
		z1 = 1.0f/(1.0f/ty1 * t + 1.0f/ty0 * (1.0f - t));
	}
	
	xmin = std::max(xmin, 0);
	xmax = std::min(xmax, xlimit);
		
	for(int x = xmin; x < xmax; x++) {
		float t = 1.0f - float(xmax - x)/float(xmax - xmin);
		
		float py = 1.0f/(1.0f/z1 * t + 1.0f/z0 * (1.0f - t));
		
		unsigned char color[3] = { colorB(colorBGR), colorG(colorBGR), colorR(colorBGR) };
		unsigned char colorSky[3];
		
		if(py < 0.1f)
			goto calc_err;
		
		map->sky()->color((y << 8)/out->width(), colorSky);
		mix(color, colorSky, py);
		
		if(steep && y >= 0 && y < viewWidth) {
			if(zBuffer[x * viewWidth + y] > py) {
				zBuffer[x * viewWidth + y] = py;
				out->setPixelFastInvY(y,x, colorSky[2], colorSky[1], colorSky[0]);
			}
		}
		else if(y >= 0 && y < viewHeight) {
			if(zBuffer[y * viewWidth + x] > py) {
				zBuffer[y * viewWidth + x] = py;
				out->setPixelFastInvY(x,y, colorSky[2], colorSky[1], colorSky[0]);
			}
		}
		
		calc_err:
		err -= dy;
		
		if(err < 0) {
			y += ystep;
			err += dx;
		}
	}
}


void Renderer::renderPolyLine(std::list<vec3> points, unsigned int colorBGR, Texture* out)
{
	static VisualEntity* sp = nullptr;
	if(sp == nullptr) {
		sp = new Decoration(Resources::i()->atlas("entities"), "bop", 0, 0, 0); 
		sp->width(0.25);
		sp->height(0.25);
		sp->visualWidth(0.25);
		sp->visualHeight(0.25);
		sp->update(0.01);
	}
		
	std::list<vec3>::iterator prev = points.begin();
	
	for(std::list<vec3>::iterator iter = ++points.begin(); iter != points.end(); iter++) {
		renderLine(*prev, *iter, colorBGR, out);
		
		sp->x(prev->x); sp->y(prev->y); sp->z(prev->z);
		renderSprite(sp, out);
		
		prev = iter;
	}
}

/*
void Renderer::renderDecals(Texture* out)
{
	struct decal {
		double x1; double y1;
		double sizeX; double sizeY;
		int tex;
	};
	
	decal decs[4] = {
		{1.0, 2.5, 1, 0, 8},
		{12.0, 11.0, 1.0, 1.0, 7},
		{6.0, 4.0, 0.0, 1.0, 3},
		{21.0, 20.0, 0.0, 1.0, 6}
	};
	
	for(int i = 0; i < 4; i++) {
		double spX = decs[i].x1 - camX;
		double spY = decs[i].y1 - camY;
		
		double invDet = 1.0 / (planeX * camDirY - camDirX * planeY);
		
		double spViewX0 = invDet * (camDirY * spX - camDirX * (spY));
		double spViewY0 = invDet * (-planeY * spX + planeX * (spY ));
		double spViewX1 = invDet * (camDirY * (spX + decs[i].sizeX) - camDirX * (spY + decs[i].sizeY));
		double spViewY1 = invDet * (-planeY * (spX + decs[i].sizeX) + planeX * (spY + decs[i].sizeY));
		
		int spScreenX0 = int((viewWidth/2) * (1 + spViewX0/spViewY0));
		int spScreenX1 = int((viewWidth/2) * (1 + spViewX1/spViewY1));
		if(spScreenX1 < spScreenX0) {
			std::swap(spScreenX0, spScreenX1);
		}

		int spWidth = abs(spScreenX1 - spScreenX0);
		
		double spx0 = spViewX0/(spViewY0 + 1);
		double spx1 = spViewX1/(spViewY1 + 1);
		if(spx1 < spx0)
			std::swap(spx1, spx0);
		
		for(int x = spScreenX0; x <= spScreenX1; x++) {
			double t = double(x - spScreenX0)/double(spWidth);
			double spx = spx0 * (1-t) + spx1 * t;
			double fx = 0, fy = 0;
			
			if(!segmentIntersect(spViewX0, spViewY0, spViewX1, spViewY1, 0, -1, spx, 0, fx, fy)) {
				continue;
			}
			
			int spHeight = abs(viewHeight/fy);
			double spViewY = fy;
			
			int texX = int(t * 32);
			if(spViewY > 0.5f && x > 0 && x < viewWidth)
			for(int y = std::max(-spHeight/2 + viewHeight/2, 0); y < std::min(spHeight/2 + viewHeight/2, viewWidth); y++) {
				 if(spViewY >= zBuffer[y * viewWidth + x])
					 continue;
				 
				float d = y - viewHeight/2 + spHeight/2;
				int texY = d * 32 / spHeight; 
				
				unsigned char* color = Resources::sheet->pixelV(texX, texY, decs[i].tex);
				if(color[0] == 0xff && color[2] == 0xff)
					continue;
				double inten = (spViewY < zNear) ? 1.0 : (zFar - std::min(spViewY, zFar))/(zFar - zNear);
				out->pixel(x, y, round(color[2] * inten), round(color[1] * inten), round(color[0] * inten)); 
			}
		}
	}
	
}

	*/
