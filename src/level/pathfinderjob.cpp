#include "pathfinderjob.h"

#include "pathfinder.h"

std::list< point2i > PathFinderJob::work()
{
	PathFinder finder(_level);
	return finder.findPath(_source, _destination);
}
