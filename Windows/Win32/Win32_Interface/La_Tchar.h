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
	using tstring = std::wstring;
	using tstringstream = std::wstringstream;
#else
#define tcout   cout
#define tcin    cin
#define ftprintf   fprintf
	using tofstream = std::ofstream;
	using tstring = std::string;
	using tstringstream = std::stringstream;
#endif

}


#endif


