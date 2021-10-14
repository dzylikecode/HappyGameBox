#include "La_Windows_Window.h"
#include <thread>
#include <Win32_Debug/La_Debug.h>
#include <windowsx.h>
#include <windows.h>
#include <mutex>
namespace
{
	HINSTANCE global_hInst;
	HWND	  global_hwnd;
	bool	  bCreated = false;
	bool      bUntilCreated = false;
	bool	  bExited = false;


	bool inlaCreateWindow(UINT width, UINT height, const TCHAR* title, WNDPROC wndProc)
	{
		WNDCLASSEX wndclassEx;
		wndclassEx.cbSize = sizeof(WNDCLASSEX);
		wndclassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wndclassEx.lpfnWndProc = wndProc ? wndProc : DefWindowProc;
		wndclassEx.cbClsExtra = 0;
		wndclassEx.cbWndExtra = 0;
		wndclassEx.hInstance = global_hInst = GetModuleHandle(nullptr);
		wndclassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndclassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndclassEx.lpszMenuName = nullptr;
		wndclassEx.lpszClassName = TEXT("La_GameBox");
		wndclassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		std::mutex mtx;
		mtx.lock();
		if (!RegisterClassEx(&wndclassEx))
		{
			DEBUG_INFO(ERR, TEXT("Register Window Class Failed !!!"));
			//End_Debug(); 不应该在此结束，而是应该在析构函数中
			bCreated = false;
			return false;
		}

		global_hwnd = CreateWindow(wndclassEx.lpszClassName, title,
			WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			width, height,
			NULL, NULL, global_hInst, nullptr);

		if (global_hwnd == NULL)
		{
			DEBUG_INFO(ERR, TEXT("Create Window Failed"));
			global_hInst = NULL;
			bCreated = false;
		}
		else
			bCreated = true;
		
		bUntilCreated = true;
		mtx.unlock();

		if (bCreated == false)
			return false;

		int wndx, wndy;
		laGetWindowPos(wndx, wndy);
		//设置的是客户区大小，而非窗口大小
		//所以有必要的调整
		RECT window_rect = { wndx, wndy, wndx + width - 1, wndy + height - 1 };

		//计算需要的窗口矩形的大小。
		//计算出的窗口矩形随后可以传送给CreateWindowEx函数，
		//用于创建一个客户区所需大小的窗口
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(global_hwnd),
			GetMenu(global_hwnd) != NULL,
			GetWindowExStyle(global_hwnd));
		// now resize the window
		MoveWindow(global_hwnd,
			wndx,
			wndy,
			window_rect.right - window_rect.left, // width
			window_rect.bottom - window_rect.top, // height
			FALSE);

		InvalidateRect(global_hwnd, nullptr, true);

		// show the window, so there's no garbage on first render
		ShowWindow(global_hwnd, SW_SHOW);

		MSG msg;

		while (TRUE)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					break;
				}

				TranslateMessage(&msg);

				DispatchMessage(&msg);
			}
		}
		mtx.lock();
		bExited = true;
		mtx.unlock();
		return true;
	}
}

bool laCreateWindow(UINT width, UINT height, const TCHAR* title, WNDPROC wndProc)
{
	if (bCreated)
	{
		DEBUG_INFO(WARN, TEXT("Window has been created!!!\n"));
		return false;
	}
	
	std::thread threadWin(inlaCreateWindow, width, height, title, wndProc);
	threadWin.detach();

	while (bUntilCreated == false)
		Sleep(5);
	if (bCreated == false)
		return false;
	return true;
}

bool laGetWindowHandle(HINSTANCE& hInst, HWND& hwnd)
{
	if (!bCreated) return false;
	hwnd = global_hwnd;
	hInst = global_hInst;
	return true;
}

bool GetWindowPos(HWND hwnd, int& x, int& y)
{
	if (hwnd == NULL) return false;
	POINT point = { 0, 0 };
	//获得客户区所在的位置
	ClientToScreen(hwnd, &point);
	x = point.x; y = point.y;
	return true;
}

bool laGetWindowPos(int& x, int& y)
{
	return GetWindowPos(global_hwnd, x, y);
}

bool laCloseWindow()
{
	SendMessage(global_hwnd, WM_CLOSE, 0, 0);
	while (bExited == false)
		Sleep(5);
	return true;
}