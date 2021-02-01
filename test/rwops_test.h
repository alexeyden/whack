#ifndef RWOPS_TEST_H
#define RWOPS_TEST_H

#include <cxxtest/TestSuite.h>
#include <string>

#include "backend/rwops_stream.h"

class TestRWops: public CxxTest::TestSuite 
{
public:
	void test_fileRead() {
		rwops_stream s("test/data/rwops.txt", "rb");
        
        std::string line;
        
        int i = 1;
        while(!s.eof() && i < 10000) {
            std::getline(s, line);
            
            TS_ASSERT_EQUALS(std::to_string(i), line);
            i++;
        }
	}
	
	void test_fileSeek() {
        rwops_stream s("test/data/rwops.txt", "rb");
        s.seekg(std::streampos(21));
        
        std::string line;
        std::getline(s, line);
        
        TS_ASSERT_EQUALS(std::string("11"), line);
    }
    
    void test_fileEof() {
        rwops_stream s("test/data/rwops.txt", "rb");
        
        size_t size = 0;
        
        char buf[256];
            
        while(!s.eof()) {
            s.read(buf, 256);
            size += s.gcount();
        } 
        
        TS_ASSERT_EQUALS(size, 48894);
    }
};

#endif
