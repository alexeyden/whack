#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <string>

#include "visualentity.h"
#include "robber.h"
#include "decoration.h"
#include "splash.h"
#include "resources/resources.h"
#include "behaviour/emptybhv.h"

class EntityFactory {
public:
	enum EntityType {
		ET_PLAYER = 0,
		ET_DECOR = 1,
		ET_ROBBER = 2,
		ET_SPLASH = 3
	};
	
	EntityFactory(const TextureAtlas* atlas) 
	 : emptyBehaviour(false), atlas(atlas) {
	}
	
	VisualEntity* create(EntityType type, const std::string& texture) {
		VisualEntity* e = nullptr;
		
		switch(type) {
			case ET_DECOR:
			{
				e = new Decoration(atlas, texture, 0, 0, 0);
			} break;
			case ET_ROBBER:
			{
				auto robber = new Robber(atlas, texture, 0, 0, 0);
				if(emptyBehaviour) {
					robber->behaviour(static_cast<Behaviour*>(new EmptyBhv(robber))); 
				}
				e = robber;
			} break;
			case ET_SPLASH: {
				e = new Splash(atlas, texture, 0, 0, 0, 1.0f);
			}
			default: break;
		}
		
		return e;
	}

	bool emptyBehaviour;
	const TextureAtlas* const atlas;
};
#endif // ENTITYFACTORY_H
