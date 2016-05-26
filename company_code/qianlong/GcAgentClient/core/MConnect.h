/************************************************************************/
/* 文件名:                                                              */
/*			core/MConnect.h												*/
/* 功能:																*/
/*			GcClient主要处理流程的第2层模块	即异步连接模块				*/
/* 特殊说明:															*/
/*			GcClient主要处理流程分为七层								*/
/*			(0)MCoreMgr 模块	主要流程入口及管理模块					*/
/*			(1)MListen	模块	负责监听								*/
/*			(2)MRcvOpen	模块	负责收用户的协议信息,根据协议填充数据	*/
/*			(3)MCONN	模块	负责异步连接GcS(或者代理服务器)			*/
/*			(4)MAGENT	模块	负责代理的认证(如果用户通过代理连接)	*/
/*			(5)MHAND	模块	负责与GcS握手建立						*/
/*			(6)MTrans	模块	负责数据转发							*/
/*			其中下层模块仅供直接上层模块调用							*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_CONNECT_H_
#define _M_CONNECT_H_


#include <windows.h>
#include <process.h>
#include "MHand.h"
#include "MBaseConfig.h"
#include "MLocalLock.h"
#include "list.h"
#include "MAgent.h"


/* 定义管理已完成收Open包的类 */
class MRcvedOpenMgr
{
public:
										MRcvedOpenMgr();
										~MRcvedOpenMgr();

public:
	int									insert(GC_PRO &gcPro);
	int									del(GC_PRO &gcPro);
	void								destroy();
	
	
private:
	CRITICAL_SECTION					m_Lock;				/** lock this struct array*/
	struct list_head					m_stUsed;			/** list used sock that accept's*/
	struct list_head					m_stFree;			/** list free sock that can accept's*/
	RCVED_OPEN_NODE						m_stRcvOpenArr[MAX_SOCK_COUNT];
};



/** CONNECT线程中使用的局部变量，仅在CONNECT线程中使用*/
typedef struct _MLOCALCONNECT_
{
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	MCONNECT_NODE						m_sDataArr[MAX_SOCK_COUNT];

										_MLOCALCONNECT_();
 										~_MLOCALCONNECT_();

	int									insert(GC_PRO &gcPro);
	void								del(MCONNECT_NODE *pNode);
	int									isFull();
	int									isEmpty();
} MLOCALCONNECT;



class MConnect
{
public:
									MConnect();
									~MConnect();

	int								init();
	void							destroy();
	void							waitExit(int *err, size_t size);
	int								insert(GC_PRO &gcPro);
	

protected:
	static unsigned int __stdcall	connThread(void * in);

	//以下私有函数，只允许在CONN线程中调用
private:
	int								_del(GC_PRO &gcPro);	
	void							_doSucess(GC_PRO &gcPro);
	void							_doFailed(GC_PRO &gcPro, int errCode);
	int								_getSockErr(SOCKET s);
	int								_upConnSrv( GC_PRO &gcPro);
	int								_getConnSrv(const GC_PRO &gcPro);

private:
	MAgent							m_clAgeModule;
	unsigned long					m_hConnHandle;
	MRcvedOpenMgr					m_stRcvedOpenMgr;
	unsigned int					m_uiConnIP;		/* 需要每次连接前都要重新设置 且为大端模式*/
	unsigned short					m_usConnPort;	/* 需要每次连接前都要重新设置 且为大端模式*/
};

#endif