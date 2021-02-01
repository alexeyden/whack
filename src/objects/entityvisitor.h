#ifndef ENTITYVISITOR_H
#define ENTITYVISITOR_H

class Decoration;
class Robber;
class Player;
class Splash;

class EntityVisitor {
public:
	virtual void visit(Decoration*) {}
	virtual void visit(Robber*) {}
	virtual void visit(Player*) {}
	virtual void visit(Splash*) {}
};

#endif //ENTITYVISITOR_H
