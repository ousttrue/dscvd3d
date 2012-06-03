#include "stdafx.h"
#include "MyWindow.h"
#include "CaptureGrabber.h"
#include "D3D.h"
#include "HUD.h"


// メッセージフィルタ処理
BOOL CMyWindow::PreTranslateMessage(MSG* pMsg)
{
    return FALSE;
}

// アイドル処理
BOOL CMyWindow::OnIdle()
{
    if(m_d3d){
        m_d3d->Render();
    }

    return FALSE;
}

LRESULT CMyWindow::OnCreate(LPCREATESTRUCT lpcs)
{
    m_d3d=std::shared_ptr<D3D>(new D3D(m_hWnd));
    m_com=std::shared_ptr<CaptureGrabber>(new CaptureGrabber);

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    return 0;
}

void CMyWindow::OnDestroy()
{
    PostQuitMessage(0);
}

void CMyWindow::OnPaint(HDC)
{
    ValidateRect(NULL);
}

LRESULT CMyWindow::OnErasebkgnd(HDC)
{
    return 0;
}

void CMyWindow::OnSize(UINT nType, CSize size)
{
    if(m_d3d){
        m_d3d->Resize(size.cx, size.cy);
    }
    //Invalidate();
}

void CMyWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch(nChar)
    {
        case ' ':
            m_d3d->Capture();
            break;

        case 'Q':
            DestroyWindow();
            break;

        default:
            {
                std::wstringstream ss;
                ss << nChar << L" down";
                HUD::Println(ss.str());
            }
    }
}

