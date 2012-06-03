#include "stdafx.h"
#include "MyWindow.h"


CAppModule _Module;  


class CCustomMessageLoop:public CMessageLoop{
public:
    CCustomMessageLoop(){}
    virtual ~CCustomMessageLoop(){}
    virtual BOOL OnIdle(int /*nIdleCount*/)
    {
        for(int i = 0; i < m_aIdleHandler.GetSize(); i++)
        {
            CIdleHandler* pIdleHandler = m_aIdleHandler[i];
            if(pIdleHandler != NULL)
                pIdleHandler->OnIdle();
        }
        return TRUE;    //continue
    }
};


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow)
{
    _Module.Init(NULL, hInstance);

    int nRet=0;
    {
        CCustomMessageLoop theLoop;
        _Module.AddMessageLoop(&theLoop);

        CMyWindow wnd;
        wnd.Create(NULL, CWindow::rcDefault,
                _T("Hello, ATL/WTL"), WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        nRet = theLoop.Run();
        _Module.RemoveMessageLoop();
    }
    _Module.Term();

    return nRet;
}

