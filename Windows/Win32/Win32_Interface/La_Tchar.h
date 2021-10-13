#ifndef __LA_TCHAR__
#define __LA_TCHAR__

#include <tchar.h>
#include <fstream>
#include <iostream>

namespace std
{
#ifdef UNICODE
#define tcout   wcout
#define tcin    wcin
#define ftprintf   fwprintf
	using tofstream = std::wofstream;
#else
#define tcout   cout
#define tcin    cin
#define ftprintf   fprintf
	using tofstream = std::ofstream;
#endif

}


#endif


