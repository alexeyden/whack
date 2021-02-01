#ifndef RWOPS_STREAM_H
#define RWOPS_STREAM_H

#include <iostream>

#include "rwops_streambuf.h"

class rwops_stream : public std::iostream
{
public:
    rwops_stream(const char* file, const char* mode);
    ~rwops_stream();

private:
    rwops_streambuf* _buf;
};

#endif // RWOPS_STREAM_H
