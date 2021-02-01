#include "levelloader.h"

#include <cmath>
#include <set>

#include "msgpack.hpp"

#include "level.h"
#include "block.h"
#include "entityvisualinfoextractor.h"
#include "lightingcalculator.h"

#include "backend/backend.h"

LevelLoader::LevelLoader(std::iostream& inout, const TextureAtlas* tileAtlas, const TextureAtlas* entityAtlas, Backend *be) :
    emptyBehaviuour(false),
    tileAtlas(tileAtlas),
    entityAtlas(entityAtlas),
    stream(inout),
    backend(be)
{

}

Level* LevelLoader::load()
{
	MsgPackSig sign;
	MsgPackHdr header;
	std::vector<std::string> textures;
	std::vector<MsgPackBlock> blocks;
	std::vector<MsgPackEntity> entities;
	std::vector<MsgPackLight> lights;
	
	const unsigned partlen = 1024;
	msgpack::unpacker pk;
	int state = 0;
	
	while(!stream.eof()) {
		pk.reserve_buffer(partlen);
		
		stream.read(pk.buffer(), partlen); 
		
        std::streamsize size = stream.gcount();
        
		if(size == 0)
			break;
		
		pk.buffer_consumed(size);
		
		msgpack::unpacked result;
		
		while(pk.next(result)) {
			switch(state) {
				case 0: //signature
				{
					result.get().convert(sign);
					
					if(sign.type != type || sign.version != version)
						return nullptr;
					
				} break;
				case 1: //header
				{
					result.get().convert(header);
				} break;
				case 2: //textures
				{
					result.get().convert(textures);
				} break;
				case 3: //blocks
				{
					result.get().convert(blocks);
				} break;
				case 4: //entities
				{
					result.get().convert(entities);
				} break;
				case 5: //lights
				{
					result.get().convert(lights);
				} break;
				default: break;
			};
			
			state++;
		}
	}
	
	Block* block_data = new Block[header.width * header.width];
	
	BlockTex sides[6] = { BT_NORTH, BT_SOUTH, BT_WEST, BT_EAST, BT_FLOOR, BT_CEILING };
	
	for(unsigned i = 0; i < header.width * header.height; i++) {
		for(int side = 0; side < 6; side++) {
			auto iter_static = tileAtlas->textureNameList().find(textures[blocks[i].textures[side]]);
			if(iter_static == tileAtlas->textureNameList().end()) {
				block_data[i].texture(sides[side], tileAtlas->animation(textures[blocks[i].textures[side]], AnimationType::IDLE));
				block_data[i].liquid = true;
			}
			else
				block_data[i].texture(sides[side], iter_static->second);
		}
		
		for(const auto& decal : blocks[i].decals) {
			block_data[i].decals(static_cast<BlockTex>(decal.side)).push_back(
				{
					decal.offsetX,
					decal.offsetY,
					decal.sizeX,
					decal.sizeY,
					tileAtlas->textureID(textures[decal.texture]),
					decal.blend
				}
			);
		}
		
		block_data[i].height = blocks[i].height;
		block_data[i].notch = blocks[i].notch; 
		block_data[i].notchHeight = blocks[i].notchHeight; 
		memcpy(&block_data[i].lightingTint, blocks[i].lights.data(), sizeof(uint32_t) * 3 * 10);
	}
	
	EntityFactory fac(entityAtlas);
	fac.emptyBehaviour = emptyBehaviuour;
	
	std::vector<VisualEntity*> ents;
	
	for(size_t i = 1; i < entities.size(); i++) {
		auto e = fac.create((EntityFactory::EntityType) entities[i].type, entities[i].texture);
		
		e->x(entities[i].x); e->y(entities[i].y); e->z(entities[i].z);
		e->width(entities[i].width); e->height(entities[i].height);
		e->visualWidth(entities[i].visualWidth); e->visualHeight(entities[i].visualHeight);
		e->dir(entities[i].dir);
		e->lighting_tint[0] = entities[i].tintR;
		e->lighting_tint[1] = entities[i].tintG;
		e->lighting_tint[2] = entities[i].tintB;
		
		ents.push_back(e);
	}
	
	std::vector<Light> lights_conv;
	for(const auto& l : lights) {
		Light nl;
		nl.x = l.x; nl.y = l.y; nl.z = l.z;
		nl.red = l.r; nl.green = l.g; nl.blue = l.b;
		nl.type = (LightType) l.type; nl.intensity = l.intensity; 
		
		lights_conv.push_back(nl);
	}
	
	Level* lvl = new Level(header.name, header.width, header.height, block_data, backend);
	lvl->description = header.description;
	
	lvl->player()->x(entities[0].x);
	lvl->player()->y(entities[0].y);
	lvl->player()->z(entities[0].z);
	lvl->player()->dir(entities[0].dir);
	
	for(VisualEntity* e : ents) {
		lvl->addEntity(e);
	}
	for(const Light& l : lights_conv) {
		lvl->addLight(l);
	}
	
	return lvl;
}

void LevelLoader::save(const Level* level)
{
	msgpack::packer<std::iostream> pk(stream);
	
	// Section 1: signature {magic, version}
	pk.pack(MsgPackSig { type, version });
	
	// Section 2: header {name, description, width, height}
	pk.pack(MsgPackHdr { level->name, level->description, level->sizeX(), level->sizeY() });
	
	//build texture table
	std::set<std::string> tex_table;
	for(const auto& texture : tileAtlas->textureNameList())
		tex_table.insert(texture.first);
	for(const auto& animation : tileAtlas->animationList()) {
		tex_table.insert(animation.first);
	}
	
	std::map<std::string, unsigned short> tex_table_map;
	
	for(const std::string& name : tex_table) {
		tex_table_map.insert(std::make_pair(name, tex_table_map.size()));
	}
	
	// Section 3: texture table [{texture_name}]
	pk.pack(tex_table);
	
	pk.pack_array(level->sizeX() * level->sizeY());
	// Section 4: blocks data [{tex_n, tex_s, tex_w, tex_e, tex_f, tex_c, decals, height, notch, notch_height}]
	for(unsigned i = 0; i < level->sizeX() * level->sizeY(); i++) {
		const Block& block = level->block(i);
		
		MsgPackBlock b;
		
		b.height = block.height;
		b.notch = block.notch;
		b.notchHeight = block.notchHeight;
		
		b.lights.resize(3 * 10);
		memcpy(b.lights.data(), &block.lightingTint, sizeof(uint32_t) * 3 * 10);
		
		BlockTex sides[6] = { BT_NORTH, BT_SOUTH, BT_WEST, BT_EAST, BT_FLOOR, BT_CEILING };
		
		for(uint_fast8_t side = 0; side < 6; side++) {
			unsigned tex_table_id;
			
			if(block.isAnimated((BlockTex) sides[side]))
				tex_table_id = tex_table_map[tileAtlas->animationName(*block.animation((BlockTex) sides[side]), AnimationType::IDLE)];
			else
				tex_table_id = tex_table_map[tileAtlas->textureName(block.texture((BlockTex) sides[side]))];
			
			b.textures[side] = tex_table_id;
			
			for(const auto& decal : block.decals(static_cast<BlockTex>(side))) {
				b.decals.push_back({ 
					decal.offsetX, decal.offsetY, decal.sizeX, decal.sizeY,
					tex_table_map[tileAtlas->textureName(decal.texture)],
					side,
					decal.blend
				});
			}
		}
		
		pk.pack(b);
	}
	
	// Section 5: entities [{type, x,y,z,w,h,vw,vh,dir,texture,tint}]
	// Player state is saved as a first entity
	
	const Player* p = level->player();
	
	pk.pack_array(level->entities().size() + 1);
	
	pk.pack(MsgPackEntity { 
		EntityFactory::ET_PLAYER,
		p->x(), p->y(), p->z(),
		0, 0, 0, 0, p->dir(), "", 0, 0, 0
	});
	
	EntityVisualInfoExtractor visInfo;
	
	for(VisualEntity* e : level->entities()) {
		auto info = visInfo.extract(e);
		
		unsigned short type = info.type;
		
		pk.pack(MsgPackEntity {
			type,
			e->x(), e->y(), e->z(),
			e->width(), e->height(), e->visualWidth(), e->visualHeight(),
			e->dir(), info.texture,
			e->lighting_tint[0], e->lighting_tint[1], e->lighting_tint[2]
		});
	}
	
	// Section 7: lights [{x,y,z,r,g,b,intensity,type}]
	
	pk.pack_array(level->lights().size());
	for(const Light& l : level->lights()) {
		pk.pack(MsgPackLight {
			l.x, l.y, l.z,
			l.red, l.green, l.blue,
			l.intensity, (uint8_t) l.type
		});
	}
}
