#include "La_StringClass.h"

using namespace std;
int main(int argc, char* argv[])
{
	ASTRING name = "ÄãºÃ";
	TSTRING tname = AToT(name);

	cout << name << endl;
	tcout << tname << endl;
	//_tprintf(tname);
	return 0;
}