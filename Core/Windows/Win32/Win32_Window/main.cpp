#include "La_Windows_Window.h"
#include <Win32_Debug/La_Debug.h>


WNDPROC oldProc;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int  cxClient, cyClient;
	HDC         hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case  WM_CREATE:
		MessageBoxPrintf(TEXT("hello"), MB_OK, TEXT("ok"));
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		Rectangle(hdc, cxClient / 8, cyClient / 8,
			7 * cxClient / 8, 7 * cyClient / 8);

		MoveToEx(hdc, 0, 0, NULL);
		LineTo(hdc, cxClient, cyClient);

		MoveToEx(hdc, 0, cyClient, NULL);
		LineTo(hdc, cxClient, 0);

		Ellipse(hdc, cxClient / 8, cyClient / 8,
			7 * cxClient / 8, 7 * cyClient / 8);

		RoundRect(hdc, cxClient / 4, cyClient / 4,
			3 * cxClient / 4, 3 * cyClient / 4,
			cxClient / 4, cyClient / 4);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int main(int argc, char* argv[])
{
	laCreateWindow(640, 480, TEXT("hello"), WndProc);
	HWND hWnd;
	HINSTANCE hinstance;
	if (laGetWindowHandle(hinstance, hWnd) == false)
	{
		DEBUG_INFO(ERR, TEXT("Get Window Handle Failed\n"));
		return 1;
	}
	
	//// 不知道为什么显示不了图形
	//oldProc =(WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

	while (1)
	{
		if (KEY_DOWN(VK_SPACE))
			InvalidateRect(hWnd, NULL, NULL);
		if (KEY_DOWN(VK_ESCAPE))
		{
			laCloseWindow();
			break;
		}

		
	}
	return 0;
}