#ifndef PATHFINDERJOB_H
#define PATHFINDERJOB_H

#include <list>

#include "util/jobrunner.h"
#include "util/math.h"

class Level;

class PathFinderJob : public Job<std::list<point2i>>
{
public:
	PathFinderJob(const Level* level,
								const point2i& src,
								const point2i& dst) :
		_source(src),
		_destination(dst),
		_level(level) {}
	
protected:
	std::list<point2i> work();
	
	point2i _source;
	point2i _destination;
	
	const Level* _level;
};

#endif // PATHFINDERJOB_H
