/************************************************************************/
/* 文件名:                                                              */
/*			core/MGlobal.h												*/
/* 功能:																*/
/*			全局变量及全局函数声明										*/
/* 特殊说明:															*/
/*			无															*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_GLOBAL_H_
#define _M_GLOBAL_H_

#include "MLog.h"
#include "MOption.h"
#include "MCoreMgr.h"
#include "MNetRand.h"





extern unsigned long				Global_DllVersion;
extern MWriteLog					Global_Log;
extern MOptionIO					Global_Option;
extern MCoreMgr						Global_CorMgr;
extern MNetRand						Global_Rand;
extern bool							Global_IsRun;



unsigned long						GcCGetDllVersion();
int									GcCStartWork(unsigned char ucRunModel, unsigned short usPort );
int									GcCEndWork();
int									GcCConfig(void * pHwnd);
int									GcCSetCallBack(void * pFunc);
int									GcCComCmd(unsigned char cType,void* p1,void* p2);










#endif