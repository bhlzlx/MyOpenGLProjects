#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include "shell/Application.h"

Application app;


typedef void (*FPS_UPDATE_CALLBACK)(unsigned int );

void print_fps( unsigned int _fps )
{
    printf("%d\n", _fps);
}

class FPS_Helper
{
private:
    double          m_uFixedFPSInterval;
    unsigned int    m_uFPS;
    unsigned int    m_uFPSCounter;

    double          m_iFPSBegin;
    double          m_iTimeUpdate;
    //
    FPS_UPDATE_CALLBACK m_onFpsUpdate;
public:
    FPS_Helper();
    void SetFixedFPS(unsigned int fps);
    bool Tick( unsigned long _tick );
    unsigned int GetFPS();
    void SetFpsCallback(FPS_UPDATE_CALLBACK callback);
};

FPS_Helper::FPS_Helper()
{
    this->m_uFixedFPSInterval = 0.032;
    this->m_iFPSBegin = 0;
    this->m_iTimeUpdate = 0;
    this->m_uFPS = 0;
    this->m_uFPSCounter = 0;
    m_onFpsUpdate = NULL;
}

void FPS_Helper::SetFixedFPS(unsigned int fps)
{
    this->m_uFixedFPSInterval = 1.0f/(float)fps;
}

bool FPS_Helper::Tick( unsigned long _tick )
{
    static double msec_curr = 0;
    msec_curr = _tick;
    double msec_diff = (float)(msec_curr - this->m_iTimeUpdate) / 1000.0f;
    double msec_fps_diff = (float)(msec_curr - this->m_iFPSBegin) / 1000.0f;

    if(msec_fps_diff >= 1.0)
    {
        //printf("fps : %d \n",fps);
        this->m_uFPS = this->m_uFPSCounter;
        this->m_uFPSCounter = 0;
        this->m_iFPSBegin = msec_curr;
        // fps¸üÐÂ»Øµ÷
        if(m_onFpsUpdate)
            m_onFpsUpdate(m_uFPS);
    }

    if(msec_diff > this->m_uFixedFPSInterval)
    {
        if(msec_diff < this->m_uFixedFPSInterval * 2)
        {
            m_iTimeUpdate = m_iTimeUpdate + m_uFixedFPSInterval * 1000;
        }
        else
        {
            m_iTimeUpdate = msec_curr;
        }
        m_uFPSCounter++;
        return true;
    }
    else
    {
        return false;
    }
}

void FPS_Helper::SetFpsCallback(FPS_UPDATE_CALLBACK callback)
{
    this->m_onFpsUpdate = callback;
}

FPS_Helper fh;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "gl_window";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "gl_window",
                          "OpenGL Core Window",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1024,
                          768,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
    
    app.Start( hwnd );
    
	ShowWindow(hwnd, nCmdShow);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            unsigned long tick = GetTickCount();
            if( fh.Tick( tick ) )
            {
                app.OnRender( tick );
            }

            Sleep(1);
        }
}
    
    app.End();
    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
			app.OnKeyEvent( wParam, Application::eKeyDown );
            break;
        }
		case WM_KEYUP:
		{
			switch(wParam)
			{
				case VK_ESCAPE:
                {
                    //PostQuitMessage(0);
                    break;
                }
				default:
				app.OnKeyEvent( wParam, Application::eKeyUp );
				break;
			}
			break;
		}
        case WM_SIZE:
        {
            WORD nWidth, nHeight;
            nWidth = LOWORD(lParam); // width of client area
            nHeight = HIWORD(lParam); // height of client area
            app.OnResize( nWidth, nHeight );
            break;
        }
		/*
		 * wParam
该参数告诉你各个虚拟键有没有被按下，可能包含多个下列值
MK_CONTROL(0x0008)
CTRL键被按下
MK_LBUTTON(0x0001)
鼠标左键被按下
MK_MBUTTON(0x0010)
鼠标中键被按下
MK_RBUTTON(0x0002)
鼠标右键被按下
MK_SHIFT(0x0004)
SHIFT键被按下
MK_XBUTTON1(0x0020)
第一个 X 按钮被按下
MK_XBUTTON2(0x0040)
第二个 X 按钮被按下
lParam
这是一个32位整数型参数。
低16位表示相对于客户区左上角的 X 坐标
高16位表示相对于客户区左上角的 Y 坐标
		 * */
		case WM_MOUSEMOVE:
		{
			short x = LOWORD(lParam);
			short y = HIWORD(lParam);
			switch( wParam )
			{
				case MK_LBUTTON:
				app.OnMouseEvent( Application::LButtonMouse, Application::MouseMove,  x, y);
				break;
				case MK_RBUTTON:
				app.OnMouseEvent( Application::RButtonMouse, Application::MouseMove,  x, y);
				break;
				case MK_MBUTTON:
				app.OnMouseEvent( Application::MButtonMouse, Application::MouseMove,  x, y);
				break;
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			short x = LOWORD(lParam);
			short y = HIWORD(lParam);
			app.OnMouseEvent( Application::LButtonMouse, Application::MouseDown,  x, y);
			break;
		}
		case WM_LBUTTONUP:
		{
			short x = LOWORD(lParam);
			short y = HIWORD(lParam);
			app.OnMouseEvent( Application::LButtonMouse, Application::MouseUp,  x, y);
			break;
		}

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
