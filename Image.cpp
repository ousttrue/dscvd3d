#include "Image.h"
#include <windows.h>


Image::Image()
: m_w(0), m_h(0)
{
}

void Image::Clear()
{
    if(m_buf.empty()){
        return;
    }
    ZeroMemory(&m_buf[0] , m_buf.size());
}

void Image::Assign(unsigned char *data, int w, int h)
{
    m_w=w;
    m_h=h;
    m_buf.assign(data, data+w*h*4);
}

void Image::Resize(int w, int h){ 
    m_w=w; 
    m_h=h; 
    m_buf.resize(w*h*4);
}

unsigned char* Image::GetSample(){ 
    if(m_buf.empty()){
        return 0;
    }
    return &m_buf[0];
}

