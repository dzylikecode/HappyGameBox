
#include <SDL.h>
#include <stdio.h>
#include "La_Bitmap.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();


//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

byte* image;
int32 width;
int32 height;
int32 bytePixels;

using namespace la;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}
	ReadImage("bitmap24.bmp", &image, &width, &height, &bytePixels);

	return success;
}



bool show()
{
	//Loading success flag
	bool success = true;

	SDL_LockSurface(gScreenSurface);

	int pitch = gScreenSurface->pitch / 4;
	int32* mem = (int32*)gScreenSurface->pixels;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			byte* cur = &image[i * width * 3 + j * 3];
			mem[i * pitch + j] = SDL_MapRGB(gScreenSurface->format, cur[2], cur[1], cur[0]);
		}
	}

	SDL_UnlockSurface(gScreenSurface);
	

	return success;
}

void close()
{

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	WriteImage("hi.bmp", image, width, height, bytePixels);
	free(image);

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!show())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Apply the image


			//Update the surface
			SDL_UpdateWindowSurface(gWindow);

			//Wait two seconds
			SDL_Delay(2000);
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}