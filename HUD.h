#pragma once
#include <sstream>
#include <windows.h>


class HUD
{
public:
    static BYTE *GetSample();
    static int GetWidth();
    static int GetHeight();
    static void SetSize(int w, int h);
    static void Println(const std::wstring &text);
    static void Update();
};

