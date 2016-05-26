/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		Global.h
	Author:			tanhg
	Create:			2009.03.16
	
	Description:	RT系统服务器全局模块
	History:		
*********************************************************************************************************/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifndef LINUXCODE

#else

typedef union _LARGE_INTEGER {
    struct {
        unsigned long LowPart;
        long HighPart;
    };
    struct {
        unsigned long LowPart;
        long HighPart;
    } u;
    long long QuadPart;
} LARGE_INTEGER;
#endif


#include "MEngine.hpp"
#include "MServiceInterActive.hpp"
#include "MSrvUnitInterface.hpp"
#include "Log.h"
#include "Request.h"
#include "Option.h"
#include "Process.h"
#include "include.h"
#include "Status.h"
#include "Control.h"
#include "OptDirMgr.h"



#define		MODULENAME				"RT系统服务器"
#define		UNITTYPE				0x2134
#define		PARESERIAL				0x0000


extern int __stdcall Global_StartServer( tagSrvUnit_PlatInterface * In, tagSrvUnit_OutInterface * Out );
extern int __stdcall Global_EndServer();

extern	unsigned long						Global_DllVersion;
extern	MCounter							Global_Counter;
extern	Option								Global_Option;
extern  Request								Global_Request;
extern  void *								Global_SrvHandle;
extern	RTFile								Global_DataIO;
extern  Status								Global_Status;
class	Control;
extern	Control								Global_Control;
extern	int									Global_UnitNo;

class MOptDirMgr;	//前置声明，解决头文件相互引用
extern  MOptDirMgr							Global_DirTran;
extern	int									MAX_TOTAL_FRAME;


#endif//__GLOBAL_H__

