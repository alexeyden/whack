#ifndef DUMMYTEST_H
#define DUMMYTEST_H

#include <cxxtest/TestSuite.h>
#include "../src/util/rangeset.h"
#include <iostream>

class TestRangeSet : public CxxTest::TestSuite 
{
public:
	void test_oneRangeIsInside() {
		RangeSet<int> r;
		r.addRange(0, 100);
		TS_ASSERT(r.isInside(0, 100) == true);
		TS_ASSERT(r.isInside(100, 101) == false);
		TS_ASSERT(r.isInside(30, 50) == true);
		TS_ASSERT(r.isInside(30, 200) == false);
		TS_ASSERT(r.isInside(200, 300) == false);
	}
	
	void test_extendingRange() {
		RangeSet<int> r;
		
		r.addRange(100, 200);
		r.addRange(150, 160);
		TS_ASSERT(r.ranges().size() == 1 && r.ranges()[0] == Range<int>(100, 200));
		
		r.addRange(50, 120);
		TS_ASSERT(r.ranges().size() == 1 && r.ranges()[0] == Range<int>(50, 200));
		
		r.addRange(150, 220);
		TS_ASSERT(r.ranges().size() == 1 && r.ranges()[0] == Range<int>(50, 220));
		
		r.addRange(0, 500);
		TS_ASSERT(r.ranges().size() == 1 && r.ranges()[0] == Range<int>(0, 500));
	}
	
	void test_multipleRangesNoIntersect() {
		RangeSet<int> r;
		
		r.addRange(100, 200);
		r.addRange(300, 400);
		r.addRange(500, 600);
		r.addRange(420, 480);
		r.addRange(0, 50);
		
		std::vector<Range<int> > correct = {
			Range<int>(0, 50),
			Range<int>(100, 200),
			Range<int>(300, 400),
			Range<int>(420, 480),
			Range<int>(500, 600)
		};
		TS_ASSERT(r.ranges() == correct);
	}
	
	void test_adjacentRanges() {
		RangeSet<int> r;
		r.addRange(100, 200);
		r.addRange(300, 400);
		r.addRange(500, 600);
		r.addRange(400, 500);
		r.addRange(201, 299);
		
		std::vector<Range<int> > correct = {
			Range<int>(100, 600),
		};
        
		TS_ASSERT(r.ranges() == correct);
	}
	
	void test_multipleRangesIntersect() {
		RangeSet<int> r;
		r.addRange(100, 200);
		r.addRange(300, 400);
		r.addRange(500, 600);
		r.addRange(400, 500);
		
		std::vector<Range<int> > correct = {
			Range<int>(100, 200),
			Range<int>(300, 600)
		};
		TS_ASSERT(r.ranges() == correct);
		
		r.addRange(0, 50);
		correct = {
			Range<int>(0, 50), Range<int>(100, 200), Range<int>(300, 600)
		};
		TS_ASSERT(r.ranges() == correct);
		
		r.addRange(650, 800);
		correct = {
			Range<int>(0, 50), Range<int>(100, 200), Range<int>(300, 600), Range<int>(650, 800)
		};
		TS_ASSERT(r.ranges() == correct);
		
		r.addRange(0, 250);
		correct = {
			Range<int>(0, 250), Range<int>(300, 600), Range<int>(650, 800)
		};
		TS_ASSERT(r.ranges() == correct);
	}
	
	void test_diff() {
		RangeSet<int> r;
		r.addRange(100, 200);
		r.addRange(300, 400);
		r.addRange(500, 600);
		
		std::vector<Range<int> > correct = {
			Range<int>(0,       100 - 1),
			Range<int>(200 + 1, 300 - 1),
			Range<int>(400 + 1, 500 - 1),
			Range<int>(600 + 1, 1000)
		};
	
		TS_ASSERT(r.diff(Range<int>(0, 1000)) == correct);
		
		correct = { Range<int>(0, 50) };
		TS_ASSERT(r.diff(Range<int>(0, 50)) == correct);
		
		correct = { Range<int>(200 + 1, 300 - 1) };
		TS_ASSERT(r.diff(Range<int>(150, 350)) == correct);
		
		RangeSet<int> r2;
		correct = { Range<int>(100, 200) };
		TS_ASSERT(r2.diff(Range<int>(100, 200)) == correct);
	}
};

#endif // DUMMYTEST_H
