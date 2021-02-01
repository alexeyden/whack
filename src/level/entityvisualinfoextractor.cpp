#include "entityvisualinfoextractor.h"

#include "objects/robber.h"
#include "objects/player.h"
#include "objects/decoration.h"

EntityVisualInfoExtractor::EntityVisualInfoExtractor()
{

}

EntityVisualInfoExtractor::EntityVisualInfo EntityVisualInfoExtractor::extract(Entity* e)
{
	e->accept(this);
	return _info;
}

void EntityVisualInfoExtractor::visit(Robber* r)
{
	_info.type = EntityFactory::ET_ROBBER;
	_info.texture = r->skinName();
	_info.atlas = "entities";
}

void EntityVisualInfoExtractor::visit(Decoration* d)
{
	_info.type = EntityFactory::ET_DECOR;
	_info.texture = d->skinName();
	_info.atlas = "entities";
}
