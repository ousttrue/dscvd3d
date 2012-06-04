#include "HUD.h"
#include "Image.h"
#include <vector>
#include <memory>
#include <tchar.h>


class TextGrid
{
    int m_col;
    int m_row;
    int m_fontsize;

    std::wstringstream m_ss;
    std::wstring m_text;

    std::shared_ptr<Image> m_image;

public:
    TextGrid()
        : m_fontsize(32), m_col(0), m_row(0), m_image(new Image)
    {
    }

    void SetSize(int w, int h)
    {
        m_image->Resize(w, h);
        m_col=w/m_fontsize*2;
        m_row=h/m_fontsize;
        m_text.clear();
    }

    void SetText(const std::wstring &text)
    {
        m_ss.clear();
        m_ss << text;
    }

    void AddText(const std::wstring &text)
    {
        m_ss << text;
    }

    BYTE* GetSample(){ 
        return m_image->GetSample();
    }
    int GetWidth(){
        return m_image->GetWidth();
    }
    int GetHeight(){
        return m_image->GetHeight();
    }

    void Update()
    {
        if(m_text==m_ss.str()){
            return;
        }

        m_text=m_ss.str();

        m_image->Clear();
        BYTE *data=m_image->GetSample();
        int pitch=m_image->GetWidth()*4;

        // フォントの生成
        LOGFONT lf = {m_fontsize, 0, 0, 0, 0, 0, 0, 0, 
            SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
            CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, _T("ＭＳ 明朝")};
        HFONT hFont=CreateFontIndirect(&lf);
        if(!(hFont)){
            return;
        }

        // デバイスコンテキスト取得
        // デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
        HDC hdc = GetDC(NULL);
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

        std::vector<BYTE> gryph;
        int x_pos=0;
        int y_pos=0;
        int col_size=m_fontsize/2;
        for(auto c=m_text.begin(); c!=m_text.end(); ++c){

            if(y_pos>=m_row*m_fontsize){
                break;
            }

            if(*c==L'\n'){
                y_pos+=m_fontsize+5;
                x_pos=0;
                continue;
            }

            if(x_pos+col_size>=m_col*col_size){
                y_pos+=m_fontsize+5;
                x_pos=0;
            }

            // フォントビットマップ取得
            TEXTMETRIC TM;
            GetTextMetrics( hdc, &TM );
            GLYPHMETRICS GM;
            CONST MAT2 Mat = {{0,1},{0,0},{0,0},{0,1}};
            DWORD size = GetGlyphOutline(hdc, *c, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);

            if(size>0){
                gryph.resize(size);
                GetGlyphOutline(hdc, *c, GGO_GRAY4_BITMAP, &GM, gryph.size(), &gryph[0], &Mat);

                // フォント情報の書き込み
                // iOfs_x, iOfs_y : 書き出し位置(左上)
                // iBmp_w, iBmp_h : フォントビットマップの幅高
                // Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階)
                int iOfs_x = x_pos+GM.gmptGlyphOrigin.x;
                int iOfs_y = y_pos+TM.tmAscent - GM.gmptGlyphOrigin.y;
                int iBmp_w = GM.gmBlackBoxX + (4-(GM.gmBlackBoxX%4))%4;
                int iBmp_h = GM.gmBlackBoxY;
                int Level = 17;
                DWORD Alpha, Color;
                for(int y=iOfs_y; y<iOfs_y+iBmp_h; y++){
                    for(size_t x=iOfs_x; x<iOfs_x+GM.gmBlackBoxX; x++){
                        Alpha = (255 * gryph[x-iOfs_x + iBmp_w*(y-iOfs_y)]) / (Level-1);
                        Color = 0x00ffffff | (Alpha<<24);
                        memcpy((BYTE*)data + pitch*y + 4*x, &Color, sizeof(DWORD));
                    }
                }
                x_pos+=iBmp_w;
            }
            else{
                x_pos+=col_size;
            }
        }

        // デバイスコンテキストとフォントハンドルの開放
        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
        ReleaseDC(NULL, hdc);
    }
};


static std::shared_ptr<TextGrid> Get()
{
    static std::shared_ptr<TextGrid> s_hud;
    if(!s_hud){
        s_hud=std::shared_ptr<TextGrid>(new TextGrid);
    }
    return s_hud;
}

BYTE *HUD::GetSample()
{
    return Get()->GetSample();
}

int HUD::GetWidth()
{
    return Get()->GetWidth();
}

int HUD::GetHeight()
{
    return Get()->GetHeight();
}

void HUD::SetSize(int w, int h)
{
    Get()->SetSize(w, h);
}

void HUD::Println(const std::wstring &text)
{
    Get()->AddText(text);
    Get()->AddText(L"\n");
}

void HUD::Update()
{
    Get()->Update();
}

