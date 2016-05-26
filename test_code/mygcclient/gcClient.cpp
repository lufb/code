#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "gcClient.h"
#include "global.h"


BOOL APIENTRY DllMain(HANDLE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved)
{
	if(Global_Handle == NULL)
		Global_Handle = hModule;

	return TRUE;
}

extern "C" _declspec(dllexport)unsigned long  __stdcall GetDllVersion()
{
	return GcCGetDllVersion();
}

extern "C" _declspec(dllexport)int  __stdcall StartWork(unsigned char ucRunModel, unsigned short usPort )
{
	return GcCStartWork(ucRunModel, usPort);
}

extern "C" _declspec(dllexport)int  __stdcall EndWork()
{
	return GcCEndWork();
}

extern "C" _declspec(dllexport)int  __stdcall Config(void * pHwnd)
{
	return GcCConfig(pHwnd);
}

extern "C" _declspec(dllexport)int  __stdcall SetCallBack(void * pFunc)
{
	return GcCSetCallBack(pFunc);
}

extern "C" _declspec(dllexport)int  __stdcall ComCmd(unsigned char cType,void* p1,void* p2)
{
	return GcCComCmd(cType, p1, p2);
}