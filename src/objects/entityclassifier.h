#ifndef ENTITY_CLASSIFIER_H
#define ENTITY_CLASSIFIER_H

#include "entityvisitor.h"
#include "entityfactory.h"

class EntityClassifer : public EntityVisitor {
public:
	EntityFactory::EntityType classify(Entity* e) {
		e->accept(this);
		return type;
	}
	
	virtual void visit(Splash*) { type = EntityFactory::ET_SPLASH; }
	virtual void visit(Player*) { type = EntityFactory::ET_PLAYER; }
	virtual void visit(Robber*) { type = EntityFactory::ET_ROBBER; }
	virtual void visit(Decoration*) { type = EntityFactory::ET_DECOR; }
	
	EntityFactory::EntityType type;
};

#endif
