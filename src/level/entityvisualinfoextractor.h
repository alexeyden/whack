#ifndef ENTITYSAVEVISITOR_H
#define ENTITYSAVEVISITOR_H

#include "objects/entity.h"
#include "objects/robber.h"
#include "objects/decoration.h"
#include "objects/player.h"
#include "objects/entityvisitor.h"
#include "objects/entityfactory.h"

#include <string>

class EntityVisualInfoExtractor : public EntityVisitor
{
public:
	struct EntityVisualInfo {
		EntityFactory::EntityType type;
		
		std::string texture;
		std::string atlas;
	};

	EntityVisualInfoExtractor();
	
	EntityVisualInfo extract(Entity* e);
	
	virtual void visit(Decoration* d);
	virtual void visit(Robber* r);
	
private:
	EntityVisualInfo _info;
};

#endif // ENTITYSAVEVISITOR_H
