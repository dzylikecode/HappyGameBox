#include <Win32_Window/La_Windows_Window.h>
#pragma comment(lib, "Win32_Window.lib")
#include <cmath>
#include <list>
const int screen_width = 640;
const int screen_height = 480;

#define RANDX	rand()%screen_width
#define RANDY	rand()%screen_height
#define RANDVEL rand()%16+1

inline COLORREF CalStarCol()
{
	// set intensity which is inversely prop to velocity for 3D effect
	// note, I am mixing equal amounts of RGB to make black -> bright white  
	int pannel = 15 * (17 - RANDVEL);
	return RGB(pannel, pannel, pannel);
}

HDC global_hdc;
HWND hwnd;

class STAR
{
public:
	int x, y;        // position of star
	int vel;        // horizontal velocity of star
	COLORREF col;   // color of star

public:
	inline void setXY(int x, int y) { this->x = x; this->y = y; }
	inline void show() { SetPixel(global_hdc, x, y, col); }
	inline void hide() { SetPixel(global_hdc, x, y, RGB(0, 0, 0)); }
	inline void move() { x += vel; if (x > screen_width) x -= screen_width; }
};

using namespace std;
list<STAR> starGroup;
#define STAR_NUM	365


void InitStars()
{
	for (auto p = starGroup.begin(); p != starGroup.end(); p++)
	{
		p->setXY(RANDX, RANDY);
		p->vel = RANDVEL;
		p->col = CalStarCol();
	}
}

void DrawStars()
{
	for (auto p = starGroup.begin(); p != starGroup.end(); p++)
	{
		p->show();
	}
}

void ClearCanvas()
{
	for (auto p = starGroup.begin(); p != starGroup.end(); p++)
	{
		p->hide();
	}
}

void MoveStars()
{
	for (auto p = starGroup.begin(); p != starGroup.end(); p++)
	{
		p->move();
	}
}


void InitLandscape()
{
	HINSTANCE hinst;
	laGetWindowHandle(hinst, hwnd);
	global_hdc = GetDC(hwnd);

	for (int i = 0; i < STAR_NUM; i++)
	{
		STAR temp;
		starGroup.push_back(temp);
	}

	InitStars();
}

void CloseLandscape()
{
	ReleaseDC(hwnd, global_hdc);
}



int main(int argc, char* argv[])
{
	laCreateWindow(screen_width, screen_height, TEXT("Star"));

	InitLandscape();

	while (1)
	{
		// get the time
		DWORD start_time = GetTickCount();
		if(KEY_DOWN(VK_ESCAPE))
			break;

		ClearCanvas();
		MoveStars();
		DrawStars();

		// lock to 30 fps
		while ((start_time - GetTickCount() < 33));
	}

	CloseLandscape();
	laCloseWindow();

	return 0;
}






