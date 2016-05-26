/************************************************************************/
/* 文件名:                                                              */
/*			GcClient.h													*/
/* 功能:																*/
/*			声明导出函数与 DllMain										*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/

//#define WIN32_LEAN_AND_MEAN
#include <windows.h>



#ifndef _GC_CLIENT_H_
#define _GC_CLIENT_H_

BOOL APIENTRY												DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

extern "C" _declspec(dllexport)unsigned long	__stdcall	GetDllVersion();
extern "C" _declspec(dllexport)int				__stdcall	StartWork(unsigned char ucRunModel, unsigned short usPort );
extern "C" _declspec(dllexport)int				__stdcall	EndWork();
extern "C" _declspec(dllexport)int				__stdcall	Config(void * pHwnd);
extern "C" _declspec(dllexport)int				__stdcall	SetCallBack(void * pFunc);
extern "C" _declspec(dllexport)int				__stdcall	ComCmd(unsigned char cType,void* p1,void* p2);



#endif
