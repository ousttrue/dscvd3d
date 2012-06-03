#pragma once


class D3D;
class CaptureGrabber;
class CMyWindow : public CWindowImpl<CMyWindow>,
    public CMessageFilter, 
    public CIdleHandler
{
    std::shared_ptr<D3D> m_d3d;
    std::shared_ptr<CaptureGrabber> m_com;

public:
    // �E�B���h�E�N���X����o�^
    DECLARE_WND_CLASS(_T("Hello"));

private:
    // ���b�Z�[�W�t�B���^����
    BOOL PreTranslateMessage(MSG* pMsg);

    // �A�C�h������
    BOOL OnIdle();

    // ���b�Z�[�W�}�b�v
    BEGIN_MSG_MAP_EX(CMyWindow)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_SIZE(OnSize)
        MSG_WM_ERASEBKGND(OnErasebkgnd)
        MSG_WM_KEYDOWN(OnKeyDown)
    END_MSG_MAP()

    LRESULT OnCreate(LPCREATESTRUCT lpcs);
    void OnDestroy();
    void OnPaint(HDC);
    LRESULT OnErasebkgnd(HDC);
    void OnSize(UINT nType, CSize size);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

