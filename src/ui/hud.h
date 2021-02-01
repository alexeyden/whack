#ifndef HUD_H
#define HUD_H

#include "objects/gunmanager.h"
#include "objects/guns.h"

class Level;
class Texture;
class TextureAtlas;
class Font;

class HUD
{
public:
	HUD(const Level* level);
	~HUD();
	
	void render(Texture* out);
    void update(float dt);
	
private:
	void renderGun(Texture* out);
	void renderGunInfo(Texture* out);
	void renderPlayerInfo(Texture* out);
	void renderNumber(Texture* out, unsigned x, unsigned y, char* number, unsigned tint);
    void renderDamageEffect(Texture *out);
	
	const Level* _level;
	
	unsigned _texCross;
	unsigned _texHealth;
	unsigned _texAmmo;
	unsigned _texNumbers[10];
	
	unsigned _texGunsIdle[GunsCount];
	unsigned _texGunsFire[GunsCount];
	
	Font* _debugFont;
	
	const TextureAtlas* _atlasUI;
	const TextureAtlas* _atlasGuns;
    
    int _prevPlayerHealth;
    float _bloodEffect;
};

#endif // HUD_H
