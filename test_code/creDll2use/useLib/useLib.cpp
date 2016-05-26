#include "mydll.h"
#include <stdio.h>
#include <windows.h>
//#pragma comment(lib, "mydll");

typedef int (*PFUN_SUM)(int, int);
typedef int (*PFUN_GET_VER)(void);

int main()
{
	HMODULE hModule = ::LoadLibrary("mydll.dll");

	PFUN_SUM newfun = (PFUN_SUM)::GetProcAddress(hModule, "sum");
	PFUN_GET_VER getVer = (PFUN_GET_VER)::GetProcAddress(hModule, "GetVer");

	int i = newfun(1,2);
	printf("result is %d\n", i);

	printf("Ver[%d]\n", getVer());

	::FreeLibrary(hModule);
	return 0;
} 

