#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <list>
#include <unordered_map>

#include "util/math.h"

class Level;

class PathFinder
{
private:
	struct Node {
		Node() : point(0, 0), g(0), h(0) {}
		Node(point2i point, float g, float h) :
		 point(point), g(g), h(h) {}
		
		point2i point;
		float g;
		float h;
		
		bool operator<(const Node& that) const {
			return (g + h) < (that.g + that.h);
		}
		
		bool operator==(const Node& that) const {
			return point == that.point;
		}
		
		size_t hash() const {
			return (size_t) ((point.x << 16) | point.y);
		}
	};
	
public:
	PathFinder(const Level* level);
	~PathFinder();
	
	std::list<point2i> findPath(const point2i& src, const point2i& dst) const;

private:
	float heuristic(const point2i& from, const point2i& dst) const;
	std::list<point2i> unwindPath(const std::unordered_map<Node, Node>& map, const Node& dst) const;
	bool canPass(const point2i& src, const point2i& dst) const;

	friend std::hash<Node>;
	
	const Level* const _level;
};

namespace std {
	template<>
	struct hash<PathFinder::Node>
	{
		size_t operator()(const PathFinder::Node& node) const noexcept {
			return node.hash();
		}
	};
}

#endif // PATHFINDER_H
