#ifndef UTILS_TEST_H
#define UTILS_TEST_H

#include <cxxtest/TestSuite.h>
#include <string>

#include "../src/util/util.h"

class TestUtils: public CxxTest::TestSuite 
{
public:
	void test_strip() {
		std::string s = "     strip me     ";
		strip(s, ' ');
		TS_ASSERT_EQUALS(s, "strip me");
		
		s = "strip me     ";
		strip(s, ' ');
		TS_ASSERT_EQUALS(s,"strip me");
		
		s = "strip me";
		strip(s, ' ');
		TS_ASSERT_EQUALS(s, "strip me");
		
		s = "     strip me";
		strip(s, ' ');
		TS_ASSERT_EQUALS(s,"strip me");
		
		s = "";
		strip(s, ' ');
		TS_ASSERT_EQUALS(s,"")
	}
	
	void test_split() {
		std::string s = "_!_!1_!2_!";
		
		auto splitted = split(s, "_!");
		TS_ASSERT_EQUALS(splitted.size(), 2);
		TS_ASSERT_EQUALS(splitted[0], "1");
		TS_ASSERT_EQUALS(splitted[1], "2");
		
		s = "1@2@3";
		splitted = split(s, "@");
		TS_ASSERT_EQUALS(splitted.size(), 3);
		TS_ASSERT_EQUALS(splitted[0], "1");
		TS_ASSERT_EQUALS(splitted[1], "2");
		TS_ASSERT_EQUALS(splitted[2], "3");
		
		s = "1";
		splitted = split(s, "@");
		TS_ASSERT_EQUALS(splitted.size(), 1);
		TS_ASSERT_EQUALS(splitted[0], "1");
	}
};

#endif // UTILS_TEST_H