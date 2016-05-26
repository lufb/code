#include <windows.h>
#include "mydll.h"
#include <stdio.h>

int Global_Version = 5;
HANDLE			Global_Handle = NULL;

BOOL APIENTRY DllMain(HANDLE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved)
{
	Global_Handle = hModule;
	return TRUE;
}

extern "C" _declspec(dllexport)int sum(int a, int b)
{
	return a+b;
}

extern "C" _declspec(dllexport)int GetVer()
{
	char		szName[256] = {'\0'};

	if(::GetModuleFileName((HMODULE)Global_Handle, szName, MAX_PATH) > 0)
	printf("Dir[%s]\n", szName);

	return Global_Version;
}
