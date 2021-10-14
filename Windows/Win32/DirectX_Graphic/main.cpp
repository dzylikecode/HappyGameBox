#include "La_Dx_Graphic_Base.h"
#include <Win32_Window/La_Windows_Window.h>
#pragma comment(lib, "Win32_Window.lib")


const int screen_width = 640;
const int screen_height = 480;

using namespace la;

int main(int argc, char* argv[])
{
	laCreateWindow(screen_width, screen_height, TEXT("DZ"));
	HWND hwnd;
	HINSTANCE hinst;
	laGetWindowHandle(hinst, hwnd);

	InitDXGraphic(hwnd, screen_width, screen_height);


	CloseDXGraphic();
	return 0;
}