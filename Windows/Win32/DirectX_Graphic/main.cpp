#include "La_Dx_Graphic_Base.h"
#include <Win32_Window/La_Windows_Window.h>
#pragma comment(lib, "Win32_Window.lib")

#define ID_TIMER 1

const int screen_width = 640;
const int screen_height = 480;
#define RandX	(rand()%screen_width)
#define RandY	(rand()%screen_height)
using namespace la;

bool bDxInit = false;


void GameBox();
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int  cxClient, cyClient;
	HDC         hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case  WM_CREATE:
		SetTimer(hwnd, ID_TIMER, 1, NULL);
		bDxInit= InitDXGraphic(hwnd, screen_width, 32, screen_height);
		return 0;

	case WM_TIMER:
		GameBox();
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;

	//case WM_PAINT:
	//	hdc = BeginPaint(hwnd, &ps);

	//	Rectangle(hdc, cxClient / 8, cyClient / 8,
	//		7 * cxClient / 8, 7 * cyClient / 8);

	//	MoveToEx(hdc, 0, 0, NULL);
	//	LineTo(hdc, cxClient, cyClient);

	//	MoveToEx(hdc, 0, cyClient, NULL);
	//	LineTo(hdc, cxClient, 0);

	//	Ellipse(hdc, cxClient / 8, cyClient / 8,
	//		7 * cxClient / 8, 7 * cyClient / 8);

	//	RoundRect(hdc, cxClient / 4, cyClient / 4,
	//		3 * cxClient / 4, 3 * cyClient / 4,
	//		cxClient / 4, cyClient / 4);

	//	EndPaint(hwnd, &ps);
	//	return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		CloseDXGraphic();
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void GameBox()
{
	if (bDxInit)
	{
		const SURFACE_PTR& back = GetBackSurface();
		PBYTE mem = 0;
		LONG lpitch = 0;

		LockSurface(back, mem, lpitch);
		COLOR* tempMem = (COLOR*)mem;
		lpitch >>= 2;
		int x = RandX;
		int y = RandY;
		for (int i = 0; i < 22507; i++)
		{
			tempMem[i] = RGB_DX(100, 100, 100);
		}

		UnlockSurface(back);

		Flush();
	}
}

int main(int argc, char* argv[])
{
	laCreateWindow(screen_width, screen_height, TEXT("DZ"), WndProc);
	HWND hwnd;
	HINSTANCE hinst;
	laGetWindowHandle(hinst, hwnd);

	while (1)
	{
		
	}

	laCloseWindow();
	return 0;
}