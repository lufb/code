// windbg.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "windbg.h"


// 20110427 tangj created  B0

unsigned long			 Global_Version = (((unsigned long )100<< 16)+0);

unsigned long WINDBG_API GetDllVersion()
{
	return Global_Version;
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


int WINDBG_API DbgInstance(int bShowDlg , int bRestart)
{
	ingest::ExceptionReport::Instance(bShowDlg,bRestart);
	return 1;
}

void	WINDBG_API DbgRelease()
{
	
}