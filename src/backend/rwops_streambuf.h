#ifndef RWOPS_STREAMBUF_H
#define RWOPS_STREAMBUF_H

#include <streambuf>
#include <ios>

#include "SDL_rwops.h"

class rwops_streambuf :  public std::streambuf
{
public:
    rwops_streambuf(const char* file, const char* mode);
    ~rwops_streambuf();
    
private:
    virtual int underflow() override;
    
    virtual int sync() override;
    virtual int overflow(int ch) override;
    
    virtual std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override; 
    virtual std::streampos seekoff(std::streamoff so, std::ios_base::seekdir dir, std::ios_base::openmode which) override;
    
private:
    SDL_RWops* _rwops;
    char* _rbuffer;
    char* _wbuffer;
    
    const size_t BUF_SIZE = 512;
};

#endif // RWOPS_STREAMBUF_H
