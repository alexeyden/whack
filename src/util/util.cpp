#include "util.h"

#include <cstdarg>
#include <cstdio>

void strip(std::string& s, char ch)
{
	while(s.size() > 0 && s[0] == ch) s.erase(0, 1);
	while(s.size() > 0 && s[s.size()-1] == ch) s.erase(s.size() - 1, 1);
}

std::vector< std::string > split(std::string str, std::string delim)
{
	std::vector<std::string> parts;
	
	std::string::size_type prev = 0, offset = 0;

	do {
		offset = str.find(delim, prev);
		if(offset != std::string::npos && offset - prev > 0)
			parts.push_back(str.substr(prev, offset - prev));
		if(offset == std::string::npos && str.size() - prev > 0)
			parts.push_back(str.substr(prev, str.size() - prev));
		
		prev = offset + delim.size();
	} while(offset != std::string::npos);
	
	return parts;
}
