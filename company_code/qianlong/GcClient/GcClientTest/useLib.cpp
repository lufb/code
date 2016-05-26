#include "mydll.h"
#include <stdio.h>
#include <windows.h>

typedef int (*DLL_MAIN)();

int main()
{
	HMODULE hModule = ::LoadLibrary("GcClient.dll");
	if(hModule == NULL)
		printf("load dll error[%d]\n", GetLastError());

	DLL_MAIN dllMain = (DLL_MAIN)::GetProcAddress(hModule, "DllTest");
	printf("begin to run dllMain\n");
	dllMain();

	::FreeLibrary(hModule);
	return 0;
} 

