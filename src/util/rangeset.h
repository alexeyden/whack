#ifndef SEGMENTCONTAINER_H
#define SEGMENTCONTAINER_H

#include <vector>
#include <utility>
#include <cstddef>
	
template<typename T> struct Range {
	public:
		Range(const T& b, const T& e) : start(b), end(e) {}
		
		
		T start, end;
};

template<typename T>
bool operator==(const Range<T>& l, const Range<T>& r) {
	return r.start == l.start && r.end == l.end;
}

/** A set of 2d ranges
 * note: range start and end are inclusive
 */
template<typename T> class RangeSet
{
public:
	RangeSet() {
		_ranges.reserve(1);
	}
	
	RangeSet(unsigned maxSize) {
		_ranges.reserve(maxSize);
	}
	
	void addRange(T start, T end) {
		if(_ranges.size() == 0) {
			_ranges.push_back(Range<T>(start, end));
			return;
		}
		
		std::pair<int, int> found = intersect(Range<T>(start-1, end+1));
		int start_index = found.first, end_index = found.second;

        // no intersections
        
		if(end_index < start_index) {
			_ranges.insert(_ranges.begin() + end_index + 1, Range<T>(start, end));
			return;
		}
	
		_ranges[start_index].start = std::min(_ranges[start_index].start, start);
		_ranges[start_index].end = std::max(_ranges[end_index].end, end);

		if(start_index == end_index)
			return;
	
		_ranges.erase(_ranges.begin() + start_index + 1, _ranges.begin() + end_index + 1);
	}
	
	/** substract all ranges in the set from given range
     */
	std::vector<Range<T> > diff(const Range<T>& r) const {
		std::vector<Range<T> > results;
		std::pair<int, int> indices = intersect(r);
		
		if(indices.second < indices.first) {
			results.push_back(r);
			return results;
		}

		if(r.start < _ranges[indices.first].start)
			results.push_back(Range<T>(r.start, _ranges[indices.first].start-1));
		
		for(int i = indices.first + 1; i <= indices.second; i++) {
			results.push_back(Range<T>(_ranges[i - 1].end+1, _ranges[i].start-1));
		}
		
		if(r.end > _ranges[indices.second].end)
			results.push_back(Range<T>(_ranges[indices.second].end+1, r.end));
	
		return results;
	}
	
	/** check if given range lays inside some range in the set */
	bool isInside(T start, T end) const {
		for(const Range<T>& it : _ranges) {
			if(start >= it.start && end <= it.end)
				return true;
		}
		
		return false;
	}
	
	const std::vector<Range<T> >& ranges() const {
		return _ranges;
	}
	
	void clear() {
		_ranges.clear();
	}

private:
	std::pair<int, int> intersect(const Range<T>& r) const {
		size_t start_index = 0, end_index;
		
		while(start_index < _ranges.size() && r.start > _ranges[start_index].end)
			++start_index;
		
		end_index = start_index - 1;
		while((end_index + 1) < _ranges.size() && r.end >= _ranges[end_index + 1].start)
			++end_index;
		
		return std::pair<int, int>(start_index, end_index);
	}
	
	std::vector<Range<T> > _ranges;
};

#endif // SEGMENTCONTAINER_H
