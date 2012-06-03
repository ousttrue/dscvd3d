#pragma once
#include <vector>
#include <list>
#include <memory>
#include <deque>

class CaptureGrabber;
class Texture;
class OpenCV;
class Image;
class D3D
{
    CComPtr<IDirect3D9> m_pD3D;

    D3DPRESENT_PARAMETERS m_d3dpp;
    CComPtr<IDirect3DDevice9> m_pd3dDevice;
    bool IsLost;

    std::shared_ptr<CaptureGrabber> m_movie;

    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Texture> m_hud;
    std::shared_ptr<Texture> m_opencvTexture;

    std::shared_ptr<OpenCV> m_opencv;
    std::shared_ptr<Image> m_opencvImage;
    std::list<std::shared_ptr<Texture> > m_textures;

    int m_captureCount;
    std::deque<std::shared_ptr<Image> > m_capturedImages;
    std::deque<std::shared_ptr<Texture> > m_capturedTextures;

public:
    D3D(HWND hWnd);
    ~D3D();
    void Resize(int w, int h);
    void Render();
    void Initialize(HWND hWnd);
    void Reset();

    void Capture();
};

