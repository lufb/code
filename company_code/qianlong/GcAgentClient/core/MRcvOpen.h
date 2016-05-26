/************************************************************************/
/* 文件名:                                                              */
/*			core/MRcvOpen.h												*/
/* 功能:																*/
/*			负责收用户的协议信息,根据协议填充数据						*/
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
/*			2013-12-20	卢富波		创建								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_RCV_OPEN_H_
#define _M_RCV_OPEN_H_

#include <windows.h>
#include <process.h>
#include "list.h"
#include "MBaseConfig.h"
#include "MConnect.h"


typedef struct _SOCKET_LIST_
{
	SOCKET							m_sock;
	struct list_head				m_listNode;
} SOCKET_LIST;

/* 定义管理accept完成后的套接字  */
class MAcceptMgr
{
public:
										MAcceptMgr();
										~MAcceptMgr();
	
	int									insert(SOCKET accSock);
	int									del(SOCKET &sock);
	void								destroy();
	
	
private:
	CRITICAL_SECTION					m_Lock;				/** lock this struct array*/
	struct list_head					m_stUsed;			/** list used sock that accept's*/
	struct list_head					m_stFree;			/** list free sock that can accept's*/
	SOCKET_LIST							m_stSockList[MAX_SOCK_COUNT];
};


/** 节点信息，仅在RCVOPEN线程中使用*/
typedef struct _MRCVOPEN_NODE_
{
	SOCKET								m_sSock;			/** socket(client sock) */
	struct list_head					m_lListNode;		/** list node */
} MRCVOPEN_NODE;


/** RCVOPEN线程中使用的局部变量，仅在RCVOPEN线程中使用*/
typedef struct _MLOCALRCVOPEN_
{
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	MRCVOPEN_NODE						m_sDataArr[MAX_SOCK_COUNT];
	
										_MLOCALRCVOPEN_();
										~_MLOCALRCVOPEN_();

	int									insert(SOCKET sock);
	void								del(MRCVOPEN_NODE *pNode);
	int									isFull();
	int									isEmpty();
} MLOCALRCVOPEN;

class MRcvOpen
{
public:
										MRcvOpen();
										~MRcvOpen();

	int									init();
	void								destroy();
	int									insert(SOCKET accSock);
	void								waitExit(int *err, size_t size);

private:
	int									_del(SOCKET &sock);
	int									_doRcvOpen(MRCVOPEN_NODE *pNode);
	int									_fillType(const char *rcvPro, const size_t size, GC_PRO &gcPro);
	int									_fillPro(const char *rcvPro, const size_t size, GC_PRO &gcPro);

protected:
	static unsigned int __stdcall		rcvOpenThread(void * in);

private:
	MAcceptMgr							m_stAcceptedMgr;	/* 管理accept后的套接字 */
	MConnect							m_clConnModule;		/* 下层模块(代理模块)*/
	unsigned long						m_hRcvOpenHandle;	/*	本层线程句柄*/
};





#endif