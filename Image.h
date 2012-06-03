#pragma once
#include <vector>


class Image
{
    int m_w;
    int m_h;
    std::vector<unsigned char> m_buf;

public:
    Image()
        : m_w(0), m_h(0)
    {
    }

    void Clear()
    {
        if(m_buf.empty()){
            return;
        }
        ZeroMemory(&m_buf[0] , m_buf.size());
    }

    void Assign(unsigned char *data, int w, int h)
    {
        m_w=w;
        m_h=h;
        m_buf.assign(data, data+w*h*4);
    }

    void Resize(int w, int h){ 
        m_w=w; 
        m_h=h; 
        m_buf.resize(w*h*4);
    }
    int GetWidth(){ return m_w; }
    int GetHeight(){ return m_h; }
    
    unsigned char* GetSample(){ 
        if(m_buf.empty()){
            return 0;
        }
        return &m_buf[0];
    }
};

