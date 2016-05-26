// MsgBoxComm.cpp : Defines the entry point for the DLL application.
//

#include "define.h"

BOOL DllInstance()
{
	WORD						Version;
	WSADATA						WsaData;
	
	Version = MAKEWORD(1,1);
	if ( WSAStartup(Version,&WsaData) != 0 )
	{
		return FALSE; //(::WSAGetLastError());
	}
	
	return TRUE;
}

int DllRelease()
{
	WSACleanup();
	return TRUE;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	BOOL		bRet = TRUE;
	switch( ul_reason_for_call ) 
	{
	case DLL_PROCESS_ATTACH:
		bRet = DllInstance();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		bRet = DllRelease();
		break;
	case DLL_THREAD_DETACH:
		break;
	}

    return bRet;
}

