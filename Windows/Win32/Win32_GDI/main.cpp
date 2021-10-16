#include <Win32_Window/La_Windows_Window.h>
#pragma comment(lib, "Win32_Window.lib")

#include <cmath>
const int screen_width = 640;
const int screen_height = 480;

#define RandX	(rand()%screen_width)
#define RandY	(rand()%screen_height)

int main(int argc, char* argv[])
{
	laCreateWindow(screen_width, screen_height, TEXT("DZ"));

	Sleep(100);
	HWND hwnd;
	HINSTANCE hinst;
	laGetWindowHandle(hinst, hwnd);

	HDC hdc = GetDC(hwnd);
	SelectObject(hdc, GetStockObject(WHITE_PEN));
	while (1)
	{
		MoveToEx(hdc, RandX, RandY, NULL);
		LineTo(hdc, RandX, RandY);
		if (KEY_DOWN(VK_ESCAPE))
		{
			break;
		}
	}
	
	ReleaseDC(hwnd, hdc);
	laCloseWindow();
	return 0;
}