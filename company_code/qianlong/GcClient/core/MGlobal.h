/************************************************************************/
/* �ļ���:                                                              */
/*			core/MGlobal.h												*/
/* ����:																*/
/*			ȫ�ֱ�����ȫ�ֺ�������										*/
/* ����˵��:															*/
/*			��															*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-25	¬����		����								*/
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