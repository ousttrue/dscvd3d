#include "stdafx.h"
#include "D3D.h"
#include "Texture.h"
#include "CaptureGrabber.h"
#include "HUD.h"
#include "Image.h"
#include "OpenCV.h"


D3D::D3D(HWND hWnd)
: IsLost(false), m_movie(new CaptureGrabber), 
    m_texture(new Texture), m_hud(new Texture), 
    m_opencv(new OpenCV), m_opencvImage(new Image), m_opencvTexture(new Texture),
    m_captureCount(0)
{
    Initialize(hWnd);

    if(SUCCEEDED(m_movie->Initialize())){
        m_texture->SetSize(m_movie->GetWidth(), m_movie->GetHeight());

        m_opencvTexture->SetSize(m_movie->GetWidth(), m_movie->GetHeight());
        m_opencvTexture->SetPosition(
                static_cast<float>(m_movie->GetWidth()), 0);
    }
    m_texture->UseAlpha(false);
    m_textures.push_back(m_texture);
    m_opencvTexture->UseAlpha(false);
    m_textures.push_back(m_opencvTexture);

    m_hud->UseVerticalFlip(true);
    m_textures.push_back(m_hud);
    m_hud->SetSize(m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight);
    HUD::SetSize(m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight);

    for(auto it=m_textures.begin(); it!=m_textures.end(); ++it){
        (*it)->SetViewport(
                D3DXVECTOR2(
                    static_cast<float>(m_d3dpp.BackBufferWidth),
                    static_cast<float>(m_d3dpp.BackBufferHeight)));
    }
}

D3D::~D3D()
{
}

void D3D::Initialize(HWND hWnd)
{
    ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));

    m_pD3D.Attach(::Direct3DCreate9( D3D_SDK_VERSION ));
    if(!m_pD3D){
        return;
    }

    m_d3dpp.Windowed = TRUE;
    m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.EnableAutoDepthStencil = 1;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    //HAL(pure vp)
    if(SUCCEEDED(m_pD3D->CreateDevice(0,D3DDEVTYPE_HAL,hWnd,
                    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, 
                    &m_d3dpp,&m_pd3dDevice))){
        return;
    }

    //HAL(soft vp)
    if(SUCCEEDED(m_pD3D->CreateDevice(0,D3DDEVTYPE_HAL,hWnd,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, 
                    &m_d3dpp,&m_pd3dDevice)))
    {
        return;
    }

    //REF
    if(SUCCEEDED(m_pD3D->CreateDevice(0,D3DDEVTYPE_REF,hWnd,
                    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, 
                    &m_d3dpp,&m_pd3dDevice)))
    {
        return;
    }

    assert(false);
}

void D3D::Reset()
{
    // ToDo:
    // D3DPOOL_DEFAULT のリソースをすべて開放する

    if(SUCCEEDED(m_pd3dDevice->Reset(&m_d3dpp))){
        IsLost=false;
    }

}

void D3D::Resize(int w, int h)
{
    if(m_d3dpp.BackBufferWidth==w 
            && m_d3dpp.BackBufferHeight==h){
        return;
    }
    m_d3dpp.BackBufferWidth = w;
    m_d3dpp.BackBufferHeight = h;

    Reset();

    for(auto it=m_textures.begin(); it!=m_textures.end(); ++it){
        (*it)->SetViewport(
                D3DXVECTOR2(
                    static_cast<float>(m_d3dpp.BackBufferWidth),
                    static_cast<float>(m_d3dpp.BackBufferHeight)));
    }
    for(auto it=m_capturedTextures.begin(); it!=m_capturedTextures.end(); ++it){
        (*it)->SetViewport(
                D3DXVECTOR2(
                    static_cast<float>(m_d3dpp.BackBufferWidth),
                    static_cast<float>(m_d3dpp.BackBufferHeight)));
    }
    m_hud->SetSize(m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight);
    HUD::SetSize(m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight);
}

void D3D::Render()
{
    HRESULT hr=m_pd3dDevice->TestCooperativeLevel();
    switch(hr)
    {
        case D3DERR_DEVICELOST:
            IsLost=true;
            Sleep(1);
            break;

        case D3DERR_DEVICENOTRESET:
            Reset();
            break;

        case S_OK:
            break;

        default:
            assert(false);
    }

    if(IsLost){
        return;
    }

    if(SUCCEEDED(m_pd3dDevice->BeginScene())){
        m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,128), 1.0f, 0);

        // capture
        if(SUCCEEDED(m_movie->Update())){
            m_texture->Assign(static_cast<BYTE*>(m_movie->GetSample()),
                    m_movie->GetWidth(), m_movie->GetHeight());
            m_texture->Draw(m_pd3dDevice);

            // process OpenCV
            m_opencvImage->Assign(static_cast<BYTE*>(m_movie->GetSample()),
                    m_movie->GetWidth(), m_movie->GetHeight());
            m_opencv->Process(m_opencvImage->GetSample(),
                    m_opencvImage->GetWidth(), m_opencvImage->GetHeight());
            m_opencvTexture->Assign(m_opencvImage->GetSample(),
                    m_opencvImage->GetWidth(), m_opencvImage->GetHeight());
            m_opencvTexture->Draw(m_pd3dDevice);
        }

        if(!m_capturedTextures.empty()){
            auto texture=m_capturedTextures.back();
            texture->SetPosition(
                    static_cast<float>(m_movie->GetWidth()*2), 0);
            texture->SetScaling(1.0f);
            texture->Draw(m_pd3dDevice);
        }

        // thumbnail
        float THUMNAIL_W=150;
        float x=0;
        float y=m_d3dpp.BackBufferHeight-THUMNAIL_W;
        for(size_t i=0; i<m_capturedTextures.size(); ++i){
            if(x+THUMNAIL_W>m_d3dpp.BackBufferWidth){
                x=0;
                y-=THUMNAIL_W;
            }
            auto image=m_capturedImages[i];
            auto texture=m_capturedTextures[i];
            int w=texture->GetWidth();
            int h=texture->GetHeight();
            texture->SetPosition(x, y);
            texture->SetScaling(THUMNAIL_W/w);
            if(!texture->IsAssigend()){
                texture->Assign(image->GetSample(),
                        image->GetWidth(), image->GetHeight());
            }
            texture->Draw(m_pd3dDevice);

            x+=THUMNAIL_W+10;
        }

        // hud
        HUD::Update();
        m_hud->Assign(HUD::GetSample(),
                HUD::GetWidth(), HUD::GetHeight());
        m_hud->Draw(m_pd3dDevice);

        m_pd3dDevice->EndScene();
        m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
    }
}

void D3D::Capture()
{
    m_captureCount++;

    // cature current image
    auto image=std::shared_ptr<Image>(new Image);
    image->Assign(static_cast<BYTE*>(m_movie->GetSample()),
            m_movie->GetWidth(), m_movie->GetHeight());
    // texture
    auto texture=std::shared_ptr<Texture>(new Texture);
    texture->UseAlpha(false);
    texture->SetSize(image->GetWidth(), image->GetHeight());
    texture->SetViewport(
            D3DXVECTOR2(
                static_cast<float>(m_d3dpp.BackBufferWidth),
                static_cast<float>(m_d3dpp.BackBufferHeight)));
    m_capturedTextures.push_back(texture);
    m_capturedImages.push_back(image);
    if(m_captureCount>8){
        m_capturedTextures.pop_front();
        m_capturedImages.pop_front();
    }
    m_opencv->Push(image);
}

