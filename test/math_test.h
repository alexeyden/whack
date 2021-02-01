#ifndef MATH_TEST_H
#define MATH_TEST_H

#include <cxxtest/TestSuite.h>
#include <string>

#include "util/math.h"

class TestMath: public CxxTest::TestSuite 
{
public:
	void test_rayAABBIntersect3D() {
		vec3 p0(2.5f, 2.5f, 1.0f + 0.01f);
		vec3 p1(4.5f, 4.5f, 1000.0f);
		
		vec3 dir((p1 - p0).normalized());
		AABB<float> aabb1(2.0f, 2.0f, 0.0f, 3.0f, 3.0f, 1.0f);
		
		TS_ASSERT_EQUALS(rayAABBIntersect3D(p0.x, p0.y, p0.z, dir.x, dir.y, dir.z, aabb1), false);
	}
};

#endif
