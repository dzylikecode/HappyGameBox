#define _LA_DEBUG_ON_
#include "La_Debug.h"
#include <iostream>
CONSOLE& consoleOut = CONSOLE::getConsole();

int main(int argc, char* argv[])
{
	DEBUG_INFO(INFO, TEXT("hello"));

	MessageBoxPrintf(TEXT("hello"), MB_OK, TEXT("%s"), TEXT("world"));



	return 0;
}