#include <La_All/La_SDL.h>


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
#define RANDX	rand()%SCREEN_WIDTH
#define RANDY	rand()%SCREEN_HEIGHT


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gWindowSurface = NULL;

void close()
{
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

using namespace la;

int num = 0;
#define NUM_COUNT	1000

int main(int argc, char* args[])
{
	//Start up SDL and create window
	gWindow = InitWindow("SetPixel", 640, 480);
	if (!gWindow)
	{
		SDL_Log("Failed to initialize!\n");
		goto FAILED;
	}

	gWindowSurface = SDL_GetWindowSurface(gWindow);

	while (1)
	{
		SDL_LockSurface(gWindowSurface);
		COLOR_PTR mem = (COLOR_PTR)gWindowSurface->pixels;
		int pitch = gWindowSurface->pitch / 4;

		for (int i = 0; i < 1000; i++)
		{
			COLOR pixel = SDL_MapRGB(gWindowSurface->format, RandByte(), RandByte(), RandByte());
			SetPixel(RANDX, RANDY, pixel, mem, pitch);
		}
		SDL_UnlockSurface(gWindowSurface);
		num++;
		SDL_UpdateWindowSurface(gWindow);
		if(num > NUM_COUNT)
			break;
	}


	close();
	return 0;
FAILED:
	//Free resources and close SDL
	close();

	return 1;
}