#define _LA_DEBUG_ON_
#include "La_Debug.h"

CONSOLE& consoleOut = CONSOLE::getConsole();

int main(int argc, char* argv[])
{
	DEBUG_INFO(INFO, TEXT("hello"));
}