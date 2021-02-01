#include "hud.h"

#include <cstring>
#include <cmath>
#include <cstdio>

#include "game/application.h"
#include "level/level.h"
#include "graphics/font.h"
#include "graphics/texture.h"
#include "resources/resources.h"
#include "util/util.h"

HUD::HUD(const Level* level) :
	_level(level)
{
	_atlasUI = Resources::i()->atlas("ui");
	_atlasGuns = Resources::i()->atlas("entities");
	
	_texHealth = _atlasUI->textureID("health");
	_texAmmo = _atlasUI->textureID("ammo");
	
	std::string pattern = "num_%";
	for(char i = '0'; i <= '9'; i++) {
		pattern[pattern.length() - 1] = i;
		_texNumbers[i - '0'] = _atlasUI->textureID(pattern);
	}
	
	_texGunsIdle[Pistol.id] = _atlasGuns->textureID("pistol_idle");
	_texGunsFire[Pistol.id] = _atlasGuns->textureID("pistol_fire");
	
	_debugFont = new Font(&Resources::i()->atlas("font")->asTexture(), 8,
		0x00d0d0d0, 0x00202020, 0x00, 1, 1
	);
	
	_texCross = _atlasUI->textureID("cross");
    
    _prevPlayerHealth = level->player()->health();
    _bloodEffect= .0f;
}

HUD::~HUD()
{
	delete _debugFont;
}

void HUD::render(Texture* out)
{
	renderGun(out);
	renderPlayerInfo(out);
	renderGunInfo(out);
    
	if(_bloodEffect > .0f)
		renderDamageEffect(out);
	
	if(APP::I()->debug) {
		char buf[64];
		snprintf(buf, 64, "Loc: (%.2f,%.2f,%.2f)",
			_level->player()->x(), _level->player()->y(), _level->player()->z()
		);
		_debugFont->drawText(out, 10, APP::I()->backend()->viewHeight() - 18 - 2*8, buf);
		
		snprintf(buf, 64, "Rot: %.2f", 
			_level->player()->dir() * 180.0f/M_PI
		);
		_debugFont->drawText(out, 10, APP::I()->backend()->viewHeight() - 18 - 1*8, buf);
	
		vec2 speed( _level->player()->speedX(), _level->player()->speedY());
		float speedNorm = speed.length();
		
		snprintf(buf, 64, "Speed: (%.2f,%.2f,%.2f) len = %.2f",
			_level->player()->speedX(), _level->player()->speedY(), _level->player()->speedZ(),
			speedNorm
		);
		_debugFont->drawText(out, 10, APP::I()->backend()->viewHeight() - 18 - 0*8, buf);
	}

	const Texture& cross = _atlasUI->texture(_texCross);
	cross.draw(out, out->width()/2 - cross.width()/2, out->height()/2 - cross.height()/2);
}

void HUD::update(float dt)
{
    int health = _level->player()->health();
    
    if(_prevPlayerHealth != health) {
        _bloodEffect = 0.5f;
        _prevPlayerHealth = health; 
    }
    
    if(_bloodEffect > 0.0f)
        _bloodEffect -= dt;
}

void HUD::renderGun(Texture* out)
{
	const GunManager::GunState state = _level->player()->gunManager().state();
	const GunType gun = _level->player()->gunManager().gunType();
    
	const Block& block = _level->block(_level->player()->x(), _level->player()->y());
	const uint32_t tint = vec2rgb(block.lightingTint.floor);
	
	switch(state) {
		case GunManager::GunState::GS_Idle:
		{	
			const Texture& tex = _atlasGuns->texture(_texGunsIdle[gun]);
			int delta = (_level->player()->viewZ() - _level->player()->z() - _level->player()->height()/2.0f)/0.05 * 5 + 5;
			
			tex.draw(out, out->width()/2 - tex.width()/2, out->height() - tex.height() + delta, tint);
		} break;
		case GunManager::GunState::GS_Firing: 
		{
			const Texture& tex = _atlasGuns->texture(_texGunsFire[gun]);
			
			float phase = _level->player()->gunManager().timeout() * M_PI/_level->player()->gunManager().get(gun).fireTime;
			int delta = tex.height()/4 * (0.5 + 0.5 * sin(phase));
			
			tex.draw(out, out->width()/2 - tex.width()/2, out->height() - tex.height() + delta, tint);
			
		} break;
		case GunManager::GunState::GS_Reloading: break;
	}
}

void HUD::renderGunInfo(Texture* out)
{
	unsigned charWidth = _atlasUI->texture(_texNumbers[0]).width();
	
	char number[32];
	
	int ammoFull = _level->player()->gunManager().gun().ammo; 
	
	snprintf(number, 32, "%03u", ammoFull);
	renderNumber(out, out->width() - charWidth*3 - 10, out->height() - 40, number, colorBGR(0x00, 0x00, 0x90));
	
	const Texture& ammo = _atlasUI->texture(_texAmmo);
	ammo.draw(out, out->width() - ammo.width() - 10, out->height() - ammo.height() - 2, colorBGR(0x30, 0x00, 0x90));
}

void HUD::renderPlayerInfo(Texture* out)
{
	char number[8];
	snprintf(number, 8, "%03u", std::max(_level->player()->health(), 0));
	renderNumber(out, 10, out->height() - 40, number, colorBGR(0x90, 0x00, 0x00));
	
	const Texture& health = _atlasUI->texture(_texHealth);
	health.draw(out, 10, out->height() - health.height() - 2, colorBGR(0x90, 0x00, 0x30));
}

void HUD::renderNumber(Texture* out, unsigned int x, unsigned int y, char* number, unsigned tint)
{
	unsigned width = _atlasUI->texture(_texNumbers[0]).width();
	
	while(*number != '\0') {
		if(*number >= '0' && *number <= '9') {
			unsigned digit = *number - '0';
			const Texture& tex = _atlasUI->texture(_texNumbers[digit]);
			tex.draw(out, x, y, tint);
		}
		
		x += width;
		
		number++;
	}
}

void HUD::renderDamageEffect(Texture* out)
{
    for(unsigned x = 0; x < out->width(); x++) {
        for(unsigned y = 0; y < out->height(); y++) {
            auto p = out->pixelPtr(x, y);
            p[2] = p[2] * (1.0f - _bloodEffect) +  0xff * _bloodEffect;
        }
    }
}
