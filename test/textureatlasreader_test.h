#ifndef TESTTEXTUREATLASREADER_H
#define TESTTEXTUREATLASREADER_H

#include <cxxtest/TestSuite.h>

#include <sstream>
#include <iostream>
#include <memory>

#include <jsoncpp/json/value.h>

#include "graphics/textureatlasreader.h"
#include "graphics/textureatlas.h"
#include "graphics/animation.h"

class TestTextureAtlasReader : public CxxTest::TestSuite 
{
private:
	std::shared_ptr<TextureAtlas> atlas;

public:
	TestTextureAtlasReader() {
		TextureAtlasReader reader("test/data/atlas.json", TextureAtlasReader::AFT_ATLAS);
		atlas = std::shared_ptr<TextureAtlas>(reader.read()); 
	}
	
	void test_sizes() {
		TS_ASSERT(atlas->textureList().size() == 20 + 1);
		TS_ASSERT(atlas->textureNameList().size() == 5);
		TS_ASSERT(atlas->animationList().size() == 2);
		TS_ASSERT(atlas->ninePatchList().size() == 1);
	}

	void test_statics() {
		TS_ASSERT_EQUALS(atlas->textureByName("torch").x(), 160);
		TS_ASSERT_EQUALS(atlas->textureByName("wooden_floor").x(), 64);
		TS_ASSERT_EQUALS(atlas->textureByName("work").x(), 96);
		TS_ASSERT_EQUALS(atlas->textureByName("grass").y(), 192);
	}
	
	void test_animations() {
		Animation anim1 = atlas->animation("zombie_new", AnimationType::IDLE);
		Animation anim2 = atlas->animation("zombie", AnimationType::IDLE);
		
		TS_ASSERT(anim1.directional == true);
		TS_ASSERT(anim2.directional == false);
		
		TS_ASSERT(atlas->texture(anim1.startFrame[AnimationViewDir::FRONT]).x() == 1);
		TS_ASSERT(atlas->texture(anim1.endFrame[AnimationViewDir::FRONT]).x() == 1);
		
		TS_ASSERT(atlas->texture(anim1.startFrame[AnimationViewDir::RIGHT]).x() == 2);
		TS_ASSERT(atlas->texture(anim1.endFrame[AnimationViewDir::RIGHT]).x() == 2);
		
		TS_ASSERT(atlas->texture(anim1.startFrame[AnimationViewDir::BACK]).x() == 3);
		TS_ASSERT(atlas->texture(anim1.endFrame[AnimationViewDir::BACK]).x() == 3);
		
		TS_ASSERT(atlas->texture(anim1.startFrame[AnimationViewDir::LEFT]).x() == 4);
		TS_ASSERT(atlas->texture(anim1.endFrame[AnimationViewDir::LEFT]).x() == 4);
		
		TS_ASSERT(atlas->texture(anim2.startFrame[0]).x() == 1); 
		TS_ASSERT(atlas->texture(anim2.endFrame[0]).x() == 2); 
	}
	
	void test_ninepatch() {
		NinePatch patch = atlas->ninePatch("button_normal");
		
		TS_ASSERT(patch.bottom.x() == 200 && patch.bottom.y() == 96);
		TS_ASSERT(patch.left.x() == 192 && patch.left.y() == 88);
		TS_ASSERT(patch.left.width() == 8 && patch.left.height() == 8);
	}
};

#endif
