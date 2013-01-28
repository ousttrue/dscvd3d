#pragma once
#include <atlbase.h>
#include <dshow.h>
#include "qedit.h"
#pragma comment(lib, "strmiids")
#include <vector>


class Com
{
public:
    Com()
    {
        CoInitialize(NULL);
    }

    ~Com()
    {
        CoUninitialize();
    }
};


template<class IEnumXXX, class IXXX>
class CEnumComPtr
{
    CComPtr<IEnumXXX> m_enumerator;

public:
    CEnumComPtr()
    {
    }

    CComPtr<IXXX> Next()
    {
        CComPtr<IXXX> n;
        if(!m_enumerator){
            return n;
        }
        m_enumerator->Next(1, &n, 0);
        return n;
    }

    IEnumXXX** operator&() throw()
    {
        return &m_enumerator;
    }

    operator IEnumXXX*() const throw()
    {
        return m_enumerator;
    }
};


class CaptureGrabber
{
    Com m_com;

    // filter graph
    CComPtr<IGraphBuilder> m_graphBuilder;
    CComQIPtr<IMediaControl> m_mediaControl;

    // src
    CComPtr<IBaseFilter> m_srcFilter;

    // dst
    CComPtr<IBaseFilter> m_dstFilter;
    CComQIPtr<ISampleGrabber> m_sampleGrabber;

    int m_w;
    int m_h;

    std::vector<BYTE> m_buffer;

public:
    CaptureGrabber();
    ~CaptureGrabber();
    HRESULT Initialize();
    int GetWidth(){ return m_w; }
    int GetHeight(){ return m_h; }
    HRESULT Update();
    void *GetSample(){ return m_buffer.empty() ? 0 : &m_buffer[0]; }
    size_t GetSampleSize(){ return m_buffer.size(); }

private:
    CComPtr<IBaseFilter> CreateDstFilter();
    CComPtr<IBaseFilter> CreateSrcFilter(const std::wstring &friendlyName);
    CComPtr<IPin> GetPin(CComPtr<IBaseFilter> filter, PIN_DIRECTION dir);
    void ShowFilters();
};

