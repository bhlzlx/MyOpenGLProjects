// OpenGL_Core.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "OpenGL_Core.h"
#include "shell/Application.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

Application theApp;

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
	wcex.lpszClassName = L"gl_window";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


	if (!RegisterClassEx(&wcex))
		return 0;

	/* create main window */
	hwnd = CreateWindowEx(0,
		L"gl_window",
		L"OpenGL Core Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1366,
		768,
		NULL,
		NULL,
		hInstance,
		NULL);

	theApp.Start(hwnd);

	ShowWindow(hwnd, nCmdShow);

	/* program main loop */
	while (!bQuit)
	{
		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
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
			theApp.OnRender(tick);
			Sleep(16);
		}
	}

	theApp.End();
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
		theApp.OnKeyEvent(wParam, Application::eKeyDown);
		break;
	}
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			//PostQuitMessage(0);
			break;
		}
		default:
			theApp.OnKeyEvent(wParam, Application::eKeyUp);
			break;
		}
		break;
	}
	case WM_SIZE:
	{
		WORD nWidth, nHeight;
		nWidth = LOWORD(lParam); // width of client area
		nHeight = HIWORD(lParam); // height of client area
		theApp.OnResize(nWidth, nHeight);
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
		switch (wParam)
		{
		case MK_LBUTTON:
			theApp.OnMouseEvent(Application::LButtonMouse, Application::MouseMove, x, y);
			break;
		case MK_RBUTTON:
			theApp.OnMouseEvent(Application::RButtonMouse, Application::MouseMove, x, y);
			break;
		case MK_MBUTTON:
			theApp.OnMouseEvent(Application::MButtonMouse, Application::MouseMove, x, y);
			break;
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		short x = LOWORD(lParam);
		short y = HIWORD(lParam);
		theApp.OnMouseEvent(Application::LButtonMouse, Application::MouseDown, x, y);
		break;
	}
	case WM_LBUTTONUP:
	{
		short x = LOWORD(lParam);
		short y = HIWORD(lParam);
		theApp.OnMouseEvent(Application::LButtonMouse, Application::MouseUp, x, y);
		break;
	}

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}