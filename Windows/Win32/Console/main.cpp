#include "La_WinConosle.h"


CONSOLE& consoleOut = CONSOLE::getConsole();

int main(int argc, char* argv[])
{
	WriteInConsole(INFO, TEXT("hello, world"));
	consoleOut.clear();
	return 0;
}