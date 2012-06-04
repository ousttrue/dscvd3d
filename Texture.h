#pragma once
#include <atlbase.h>
#include <d3d9.h>
#include <d3dx9.h>


class CaptureGrabber;
class Texture
{
    CComPtr<IDirect3DTexture9>  m_pTexture;

    D3DXVECTOR2 m_viewport;
    D3DXMATRIX m_projection;
    D3DXMATRIX m_world;

    int m_w;
    int m_h;

    bool m_useAlpha;
    bool m_useVerticalFlip;
    bool m_isAssigened;

public:
    Texture();
    void Release();
    void UseAlpha(bool use){ m_useAlpha=use; }
    void UseVerticalFlip(bool use){ m_useVerticalFlip=use; }
    void SetSize(int w, int h);
    int GetWidth(){ return m_w; }
    int GetHeight(){ return m_h; }
    bool IsAssigend(){ return m_isAssigened; }
    void SetPosition(float x, float y);
    void SetScaling(float factor);
    void SetViewport(const D3DXVECTOR2 &viewport);
    void Initialize(CComPtr<IDirect3DDevice9> pD3D);
    void Assign(BYTE *data, int w, int h);
    void Draw(CComPtr<IDirect3DDevice9> pD3D);
};

