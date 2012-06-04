#include "Texture.h"
#include <assert.h>


struct Vertex
{
    float x;
    float y;
    float z;
    float u;
    float v;
};
#define CUSTOM_FVF D3DFVF_XYZ | D3DFVF_TEX1


Texture::Texture()
: m_pTexture(0), m_w(0), m_h(0), m_useAlpha(true), m_useVerticalFlip(false),
    m_isAssigened(false)
{
    D3DXMatrixIdentity(&m_world);
}

void Texture::Release()
{
    m_pTexture=0;
}

void Texture::SetViewport(const D3DXVECTOR2 &viewport)
{
    m_viewport=viewport;
    D3DXMatrixOrthoOffCenterLH(&m_projection,
            0, m_viewport.x,
            0, m_viewport.y,
            0, 1.0f);
}

void Texture::SetSize(int w, int h)
{
    if(m_w==w && m_h==h){
        return;
    }
    m_w=w;
    m_h=h;
    Release();
}

void Texture::Initialize(CComPtr<IDirect3DDevice9> pD3D)
{
    if(m_pTexture){
        return;
    }

    if(m_w==0 || m_h==0){
        return;
    }

	if(FAILED(pD3D->CreateTexture(m_w, m_h, 1, 
		0, 
		D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		&m_pTexture, NULL))){
        assert(false);
    }
}

void Texture::Assign(BYTE *data, int w, int h)
{
    if(!m_pTexture){
        return;
    }

    if(w==m_w && h==m_h){
        // テクスチャを更新
        D3DLOCKED_RECT lockRect;
        if(FAILED(m_pTexture->LockRect(0, &lockRect, NULL, D3DLOCK_DISCARD))){
            return;
        }

        if(w*4==lockRect.Pitch){
            memcpy(static_cast<BYTE*>(lockRect.pBits), data, m_w*m_h*4);
        }
        else{
			assert(false);
        }

        m_pTexture->UnlockRect(0);

        m_isAssigened=true;
    }
}

void Texture::SetPosition(float x, float y)
{
    m_world._41=x;
    m_world._42=y;
}

void Texture::SetScaling(float factor)
{
    m_world._11=factor;
    m_world._22=factor;
}

void Texture::Draw(CComPtr<IDirect3DDevice9> pD3D)
{
    if(!m_pTexture){
        Initialize(pD3D);
    }

    //テクスチャ設定
    pD3D->SetTexture(0, m_pTexture);

    pD3D->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pD3D->SetRenderState(D3DRS_ZENABLE,FALSE);
    pD3D->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
    pD3D->SetRenderState(D3DRS_LIGHTING,FALSE);

    if(m_useAlpha){
        pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pD3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        pD3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); 

        // テクスチャセット
        pD3D->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE ); 
        pD3D->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_MODULATE );
        pD3D->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE ); 
        pD3D->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        pD3D->SetTextureStageState(0, D3DTSS_ALPHAOP , D3DTOP_MODULATE ); 
        pD3D->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE ); // 板ポリのα値を利用
    }
    else{
        pD3D->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        pD3D->SetTextureStageState(0, D3DTSS_COLOROP , D3DTOP_SELECTARG1);
    }

    float w=static_cast<float>(m_w);
    float h=static_cast<float>(m_h);

    float v0=m_useVerticalFlip ? 1.0f : 0.0f;
    float v1=m_useVerticalFlip ? 0.0f : 1.0f;
    Vertex vertices[]={
        0, h, 0, 0, v1,
        0, 0, 0, 0, v0,
        w, h, 0, 1, v1,
        w, 0, 0, 1, v0,
    };

    pD3D->SetTransform(D3DTS_PROJECTION, &m_projection);
    pD3D->SetTransform(D3DTS_WORLD, &m_world);

    pD3D->SetFVF(CUSTOM_FVF);
    pD3D->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,
            2,
            &vertices,
            sizeof(Vertex));
}

