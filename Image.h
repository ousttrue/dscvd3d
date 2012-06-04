#pragma once
#include <vector>


class Image
{
    int m_w;
    int m_h;
    std::vector<unsigned char> m_buf;

public:
    Image();

    void Clear();

    void Assign(unsigned char *data, int w, int h);

    void Resize(int w, int h);
    int GetWidth(){ return m_w; }
    int GetHeight(){ return m_h; }

    unsigned char* GetSample();
};

