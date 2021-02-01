#include "rwops_stream.h"

rwops_stream::rwops_stream(const char* file, const char* mode)
{
    _buf = new rwops_streambuf(file, mode);
    
    init(_buf);
}

rwops_stream::~rwops_stream()
{
    delete _buf;
}


