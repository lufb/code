/**
 *	global.h
 *	����: �������������õĺ���
 *
 *		CopyRight @¬���� <1164830775@qq.com>
 *
 *	�޸���ʷ
 *		2013-11-21	¬����	����
 *			¬���� <1164830775@qq.com>
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "MOption.h"
#include "MthreadMgr.h"

extern HANDLE						Global_Handle;
extern unsigned long				Global_DllVersion;
extern MOptionIO					Global_Option;
extern MThreadMgr					Global_ThreadMgr;



unsigned long  GcCGetDllVersion();

int  GcCStartWork(unsigned char ucRunModel, unsigned short usPort );

int  GcCEndWork();

int  GcCConfig(void * pHwnd);

int  GcCSetCallBack(void * pFunc);

int  GcCComCmd(unsigned char cType,void* p1,void* p2);




#endif