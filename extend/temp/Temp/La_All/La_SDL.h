#pragma once
#include <SDL.h>
#include <SDL_log.h>
#include <cmath>
namespace la
{
	typedef unsigned int	UINT;
	typedef UINT			COLOR;
	typedef COLOR*			COLOR_PTR;
	typedef unsigned char	BYTE;

	inline 
		BYTE RandByte() { return rand() % 256; }

	inline
		void SetPixel(int x, int y, COLOR color, COLOR_PTR mem, int pitch)
	{
		mem[y * pitch + x] = color;
	}

	//bool CreateWindow(const char* titile, )
	inline
		SDL_Window* InitWindow(const char* title,
		int w, int h)
	{
		SDL_Window* gWindow = NULL;
		//Initialization flag
		bool success = true;

		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create window
			gWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
			if (gWindow == NULL)
			{
				SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
				success = false;
			}
		}
		if (success) return gWindow;
		return NULL;
	}




}