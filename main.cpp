#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <memory>
#include <functional>
#include <string>
#include "D3D.h"
#include "CaptureGrabber.h"
typedef std::basic_string<TCHAR> tstring;

class Window
{
    HWND hwnd_;
    std::function<void(int, int)> onResize_;
	std::function<bool(int)> onKeydown_;
public:
    Window()
        : hwnd_(0)
        {
        }

    void setHwnd(HWND hwnd)
    {
        hwnd_=hwnd;
    }
    HWND getHwnd()
    {
        return hwnd_;
    }

    void show()
    {
        ShowWindow(hwnd_, SW_SHOWNORMAL);
        UpdateWindow(hwnd_);
    }
    
	void SetOnKeydown(std::function<bool(int)> callback)
	{
		onKeydown_=callback;
	}

    void SetOnResize(std::function<void(int,int)> callback)
    {
        onResize_=callback;
    }

    LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch(message)
        {
            case WM_PAINT:
                ValidateRect(hwnd, NULL);
                return 0;

            case WM_ERASEBKGND:
                return 1;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
                if(onResize_){
                    onResize_(LOWORD(lParam), HIWORD(lParam));
                    return 0;
                }
                break;

			case WM_KEYDOWN:
				if(onKeydown_){
					if(onKeydown_(wParam)){
						return 0;
					}
				}
				break;  // “Á‚Éˆ—‚µ‚È‚¢ƒL[‚ÍDefWindowProc()‚É”C‚¹‚é
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    static LRESULT CALLBACK WndProcProxy(
            HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ((Window*)GetWindowLongPtr(
                    hwnd, GWL_USERDATA))->WndProc(hwnd, message, wParam, lParam);
    }
};


class WindowFactory
{
public:
    WindowFactory()
    {
    }

    bool register_class(const tstring &className)
    {
        WNDCLASSEX wndclass;
        ZeroMemory(&wndclass, sizeof(wndclass));
        wndclass.cbSize=sizeof(WNDCLASSEX);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = WindowFactory::WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = GetModuleHandle(NULL);
        wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = className.c_str();
        return RegisterClassEx(&wndclass)!=0;
    }

    std::shared_ptr<Window> 
        create(const tstring &title, const tstring &className)
        {
            std::shared_ptr<Window> window(new Window());
            HWND hwnd=CreateWindowEx(0,
                    className.c_str(),
                    title.c_str(),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    NULL,
                    NULL,
                    GetModuleHandle(NULL),
                    window.get());
            if(hwnd==0){
                return std::shared_ptr<Window>();
            }
            window->setHwnd(hwnd);
            return window;
        }


    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch(message)
        {
            case WM_CREATE:
                {
                    Window *window=(Window*)((LPCREATESTRUCT)lParam)->lpCreateParams;
                    SetWindowLongPtr(hwnd, GWL_USERDATA, (LONG_PTR)window);
                    SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)Window::WndProcProxy);
                    return window->WndProc(hwnd, message, wParam, lParam);
                }

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
};


int WINAPI WinMain(
        HINSTANCE hInstance ,
        HINSTANCE hPrevInstance ,
        LPSTR lpCmdLine ,
        int nCmdShow ) {

    auto CLASS_NAME=_T("HelloWindow");
    auto WINDOW_NAME=_T("Hello Window");

    WindowFactory factory;
    if(!factory.register_class(CLASS_NAME)){
        return 1;
    }
    std::shared_ptr<Window> window=factory.create(WINDOW_NAME, CLASS_NAME);
    if(!window){
        return 2;
    }
    window->show();

    auto d3d=std::shared_ptr<D3D>(new D3D(window->getHwnd()));
    auto com=std::shared_ptr<CaptureGrabber>(new CaptureGrabber);

    window->SetOnResize([d3d](int w, int h){
            d3d->Resize(w, h);
            });
	window->SetOnKeydown([d3d](int key)->bool{
		if(key == VK_SPACE)
		{
			d3d->Capture();
			return true;
		}
		return false;
	});

    MSG msg;
    bool isLoop=true;
    while(isLoop){
        while(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)){
            if (!GetMessage(&msg,NULL,0,0)){
                // WM_QUIT
                isLoop=false;
                break;
            }
            // dispatch message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // idle func
        d3d->Render();
    }

    return msg.wParam;
}

