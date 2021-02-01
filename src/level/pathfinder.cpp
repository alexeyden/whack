#include "pathfinder.h"

#include <cmath>
#include <unordered_set>
#include <set>
#include <unordered_map>

#include "level.h"

PathFinder::PathFinder(const Level* level)
 : _level(level)
{

}

PathFinder::~PathFinder()
{

}

float PathFinder::heuristic(const point2i& from, const point2i& dst) const
{
	return (dst - from).manhattanLength();
}

bool PathFinder::canPass(const point2i& src, const point2i& dst) const
{
	const float maxUp = 0.25f;
	const float maxDown = -1.2f;
	
	bool insideBounds = dst.x >= 0 && dst.y >= 0 &&
        dst.x < (int) _level->sizeX() && dst.y < (int) _level->sizeY();
	
	if(!insideBounds)
		return false;
											
	const auto& srcBlock = _level->block(src.x, src.y);
	const auto& dstBlock = _level->block(dst.x, dst.y);
	
	float heightDelta = dstBlock.standHeight() - srcBlock.standHeight();
	bool validHeight = heightDelta > maxDown && heightDelta < maxUp;
	
	return validHeight; 
}

std::list< point2i > PathFinder::unwindPath(const std::unordered_map< PathFinder::Node, PathFinder::Node >& map, const PathFinder::Node& dst) const
{
	std::list<point2i> path;
	
	PathFinder::Node n = dst;
	
	path.push_front(dst.point);
	
	while(map.find(n) != map.end()) {
		path.push_front(n.point);
		n = map.at(n);
	}
	
	return path;
}

std::list< point2i > PathFinder::findPath(const point2i& src, const point2i& dst) const
{
	std::unordered_set<Node> closed;
	std::set<Node> open;
	std::unordered_map<Node, Node> transitions;
	
	const std::list<point2i> neighbors {
		point2i(-1, 0), point2i(+1, 0), point2i(0, -1), point2i(0, +1),
		point2i(-1, -1), point2i(-1, +1), point2i(+1, -1), point2i(+1, +1)
	};
	
	open.insert(Node(src, 0, heuristic(src, dst)));
	
	while(!open.empty()) {
		Node current = *open.begin();
		open.erase(open.begin());
		
		if(current.point == dst) {
			return unwindPath(transitions, current);
		}
		
		closed.insert(current);
		
		for(const point2i& nbdelta : neighbors) {
			point2i neighbor = current.point + nbdelta;
			
			if(!canPass(current.point, neighbor))
				continue;
			
			if(nbdelta.x != 0 && nbdelta.y != 0 && !(
				canPass(current.point, point2i(current.point.x + nbdelta.x, current.point.y)) &&
				canPass(current.point, point2i(current.point.x, current.point.y + nbdelta.y))
			)) continue;
			
			Node node(neighbor, 0, 0);
			
			if(closed.find(node) != closed.end())
				continue;
			
			float score = current.g + ((nbdelta.x != 0 && nbdelta.y != 0) ? 1.41f : 1.0f);
			
			const auto& foundNodeIter = std::find(open.begin(), open.end(), node);
			
			if(foundNodeIter == open.end()) {
				node.g = score;
				node.h = heuristic(node.point, dst);
				open.insert(node);
				transitions[node] = current;
			}
			else {
				node = *foundNodeIter;
				
				if(score < node.g) {
					node.g = score;
					transitions[node] = current;
					
					open.erase(foundNodeIter);
					open.insert(node);
				}
			}
		}
	}
	
	return std::list<point2i>();
}
