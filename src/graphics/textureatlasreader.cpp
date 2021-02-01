#include "textureatlasreader.h"

#include <string>
#include <algorithm>
#include <byteswap.h>

#include "json/value.h"
#include "json/json.h"

#include "backend/rwops_stream.h"

#include "util/util.h"

#include "animation.h"

TextureAtlasReader::TextureAtlasReader(const char* f, TextureAtlasReader::AtlasFileType type):
    file(f),
    fileType(type)
{
}

TextureAtlasReader::~TextureAtlasReader()
{

}

TextureAtlas* TextureAtlasReader::read()
{
	if(fileType == TextureAtlasReader::AFT_ATLAS)
		return readFromAtlas(file);
	else
		return readFromImage(file);
}

TextureAtlas* TextureAtlasReader::readFromAtlas(const char* f)
{
    rwops_stream atlas_stream(f, "r");
    
	Json::Value root;
	atlas_stream >> root;
    
	std::string fname = root["image-file"].asString();
    
    rwops_stream bmp_stream(fname.c_str(), "rb");
    
	atlas = readFromImage(fname.c_str());
	
	Json::Value static_list = root["static"];
	for(int i = 0; i < (int) static_list.size(); i++) {
		std::string name = static_list[i]["name"].asString();
		Json::Value rect = static_list[i]["rect"];
		
		unsigned x = rect[0].asUInt(), y = rect[1].asUInt(), w = rect[2].asUInt(), h = rect[3].asUInt();
		atlas->addStatic(name, x, y, w, h);
	}
	
	Json::Value anim_list = root["animation"];
	for(int i = 0; i < (int) anim_list.size(); i++) {
		std::string name = anim_list[i]["name"].asString();
		
		std::map<std::string, AnimationType> type_map {
			std::pair<std::string, AnimationType>("idle", AnimationType::IDLE),
			std::pair<std::string, AnimationType>("walk", AnimationType::WALK),
			std::pair<std::string, AnimationType>("attack", AnimationType::ATTACK),
			std::pair<std::string, AnimationType>("death", AnimationType::DEATH)
		};
	
		AnimationType type = type_map[anim_list[i]["type"].asString()];
		
		bool directional = anim_list[i]["directional"].asBool();
		
		if(directional) {
			Json::Value frames = anim_list[i]["frames"];
			
			std::vector<Json::Value> frame_nodes(4);
			frame_nodes[AnimationViewDir::LEFT] = frames["left"];
			frame_nodes[AnimationViewDir::RIGHT] = frames["right"];
			frame_nodes[AnimationViewDir::FRONT] = frames["front"];
			frame_nodes[AnimationViewDir::BACK] = frames["back"];
			
			Animation anim;
			anim.directional = true;
			
			for(int j = 0; j < 4; j++) {
				unsigned start = 0, end = 0;
				
				for(int k = 0; k < (int) frame_nodes[j].size(); k++) {
					unsigned id = atlas->addTexture(
						frame_nodes[j][k][0].asUInt(), frame_nodes[j][k][1].asUInt(),
						frame_nodes[j][k][2].asUInt(), frame_nodes[j][k][3].asUInt()
					);
					
					if(k == 0)
						start = id; 
					if(k == (int) frame_nodes[j].size() - 1)
						end = id;
				}
				
				anim.startFrame[j] = start;
				anim.endFrame[j] = end;
			}
			
			atlas->addAnimation(name, type, anim);
		}
		else {
			Json::Value frames = anim_list[i]["frames"];
			unsigned start = 0, end = 0;
			
			for(int j = 0; j < (int) frames.size(); j++) {
				unsigned id = atlas->addTexture(frames[j][0].asUInt(), frames[j][1].asUInt(), frames[j][2].asUInt(), frames[j][3].asUInt());
				
				if(j == 0)
					start = id; 
				if(j == (int) frames.size() - 1)
					end = id;
			}
			
			atlas->addAnimation(name, type, Animation(start, end));
		}
	}
	
	Json::Value ninepatch_list = root["ninepatch"];
	for(int i = 0; i < (int) ninepatch_list.size(); i++) {
		std::string name = ninepatch_list[i]["name"].asString();
		
		std::map<std::string, unsigned> np_map = {
			std::pair<std::string, unsigned>("top-left", NinePatchPart::NP_PART_TOPLEFT),
			std::pair<std::string, unsigned>("top-right", NinePatchPart::NP_PART_TOPRIGHT),
			std::pair<std::string, unsigned>("bottom-left", NinePatchPart::NP_PART_BOTTOMLEFT),
			std::pair<std::string, unsigned>("bottom-right", NinePatchPart::NP_PART_BOTTOMRIGHT),
			std::pair<std::string, unsigned>("left", NinePatchPart::NP_PART_LEFT),
			std::pair<std::string, unsigned>("right", NinePatchPart::NP_PART_RIGHT),
			std::pair<std::string, unsigned>("top", NinePatchPart::NP_PART_TOP),
			std::pair<std::string, unsigned>("bottom", NinePatchPart::NP_PART_BOTTOM),
			std::pair<std::string, unsigned>("center", NinePatchPart::NP_PART_CENTER)
		};
	
		unsigned np_parts[9];
		
		for(auto& pair : np_map) {
			Json::Value rect = ninepatch_list[i][pair.first];
			unsigned id = atlas->addTexture(rect[0].asUInt(), rect[1].asUInt(), rect[2].asUInt(), rect[3].asUInt());
			
			np_parts[pair.second] = id;
		}
		
		NinePatch ninepatch(atlas,
			np_parts[0], np_parts[1], np_parts[2], np_parts[3], 
			np_parts[4], np_parts[5], np_parts[6], np_parts[7], 
			np_parts[8]
		);
		atlas->addNinePatch(name, ninepatch);
	}
	
	return atlas;
}

TextureAtlas* TextureAtlasReader::readFromImage(const char* fname)
{
    unsigned short x = 1;
    bool is_bigendian = *((uint8_t *) &x) == 0;
 
    rwops_stream is(fname, "rb");
    
    unsigned int offset = 0;
    is.seekg(10, std::ios_base::beg);
    is.read((char*) &offset, 4);
    
    unsigned int image_width = 0;
    is.seekg(18, std::ios_base::beg);
    is.read((char*) &image_width, 4);
    
    unsigned int image_height = 0;
    is.seekg(22, std::ios_base::beg);
    is.read((char*) &image_height, 4);

    if(is_bigendian) {
        offset = bswap_32(offset);
        image_width = bswap_32(image_width);
        image_height = bswap_32(image_height);
    }
    
    unsigned char* d = new unsigned char[image_height * image_height * 3];
    is.seekg(offset, std::ios_base::beg);
    is.read((char*) d, image_height * image_width * 3);
    
	return new TextureAtlas(d, image_width, image_height);
}
