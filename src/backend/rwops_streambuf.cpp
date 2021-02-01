#include "rwops_streambuf.h"

rwops_streambuf::rwops_streambuf::rwops_streambuf(const char* file, const char* mode)
{
    _rwops = SDL_RWFromFile(file, mode);
    
    _rbuffer = new char[BUF_SIZE];
    _wbuffer = new char[BUF_SIZE];
    
    setg(_rbuffer, _rbuffer + BUF_SIZE, _rbuffer + BUF_SIZE);
    setp(_wbuffer, _wbuffer + BUF_SIZE);
}

rwops_streambuf::rwops_streambuf::~rwops_streambuf()
{
    SDL_RWclose(_rwops);
    
    delete[] _rbuffer;
    delete[] _wbuffer;
    
}

int rwops_streambuf::underflow()
{
    if(gptr() == egptr()) {
        size_t size = SDL_RWread(_rwops, _rbuffer, 1, BUF_SIZE);
        
        if(size == 0)
            return std::char_traits<char>::eof();
        
        setg(_rbuffer, _rbuffer, _rbuffer + size); 
    }
    
    return std::char_traits<char>::to_int_type(*gptr());
}

int rwops_streambuf::overflow(int ch)
{
    if(ch != std::char_traits<char>::eof()) {
        size_t size = SDL_RWwrite(_rwops, _wbuffer, 1, BUF_SIZE);
        setp(_wbuffer + BUF_SIZE - size * BUF_SIZE, _wbuffer + BUF_SIZE);
        
        return std::char_traits<char>::to_int_type(ch);
    }
    
    return std::char_traits<char>::eof();
}

int rwops_streambuf::sync()
{
    size_t size = pptr() - pbase();
    
    while (size > 0) {
        size -= SDL_RWwrite(_rwops, _wbuffer, 1, BUF_SIZE);
    }
    
    setp(_wbuffer, _wbuffer + BUF_SIZE);
    
    return 0;
}

std::streampos rwops_streambuf::seekpos(std::streampos sp, std::ios_base::openmode which)
{
    (void) which;
    
    std::streamoff offset = sp;
    Sint64 pos = SDL_RWseek(_rwops, offset, RW_SEEK_SET);
    
    setg(_rbuffer, _rbuffer + BUF_SIZE, _rbuffer + BUF_SIZE);
    setp(_wbuffer, _wbuffer + BUF_SIZE);
    
    return std::streampos(std::streamoff(pos));
}

std::streampos rwops_streambuf::seekoff(std::streamoff so, std::ios_base::seekdir dir, std::ios_base::openmode which)
{
    (void) which;
    
    int seek = RW_SEEK_SET;
    
    if(dir == std::ios_base::cur)
        seek = RW_SEEK_CUR;
    else if(dir == std::ios_base::end)
        seek = RW_SEEK_END;
    
    Sint64 pos =  SDL_RWseek(_rwops, so, seek);
    
    setg(_rbuffer, _rbuffer + BUF_SIZE, _rbuffer + BUF_SIZE);
    setp(_wbuffer, _wbuffer + BUF_SIZE);
    
    return std::streampos(std::streamoff(pos));
}
