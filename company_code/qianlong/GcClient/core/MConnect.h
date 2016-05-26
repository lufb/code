/************************************************************************/
/* 文件名:                                                              */
/*			core/MConnect.h												*/
/* 功能:																*/
/*			GcClient主要处理流程的第2层模块	即异步连接模块				*/
/* 特殊说明:															*/
/*			GcClient主要处理流程分为五层								*/
/*			(0)MCoreMgr 模块	主要流程入口							*/
/*			(1)MListen	模块	负责监听								*/
/*			(2)MConnect	模块	负责异步connect及channel建				*/
/*			(3)MHand	模块	负责握手建立							*/
/*			(4)MTrans	模块	负责数据转发							*/
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
	int									delOneFromUsed(SOCKET &sock);
	void								destroy();
	
	
#ifdef _DEBUG
	//below is for test
	void								printList(struct list_head* head);
	void								printFree();
	void								printUsed();
#endif
	
private:
	
	
private:
	CRITICAL_SECTION					m_Lock;				/** lock this struct array*/
	struct list_head					m_stUsed;			/** list used sock that accept's*/
	struct list_head					m_stFree;			/** list free sock that can accept's*/
	SOCKET_LIST							m_stSockList[MAX_SOCK_COUNT];
};


/** CONNECT线程中完成CONNECT的节点信息，仅在CONNECT线程中使用*/
typedef struct _MCONNECT_NODE_
{
	SOCKET								m_sCliSock;			/** client socket */
	SOCKET								m_sSrvSock;			/** server socket */
	struct list_head					m_lListNode;		/** list node */
} MCONNECT_NODE;

/** CONNECT线程中使用的局部变量，仅在CONNECT线程中使用*/
typedef struct _MLOCALCONNECT_
{
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	MCONNECT_NODE						m_sDataArr[MAX_SOCK_COUNT];

										_MLOCALCONNECT_();
 										~_MLOCALCONNECT_();
} MLOCALCONNECT;



class MConnect
{
public:
									MConnect();
									~MConnect();

	int								init();
	void							destroy();
	void							waitExit(int *err, size_t size);
	int								insert(SOCKET accSock);
	

protected:
	static unsigned int __stdcall	connThread(void * in);

private:
	int								_del(SOCKET &sock);
	//以下私有函数，只允许在CONN线程中调用
	int								_insertToLocal(MLOCALCONNECT &local, SOCKET cliSock, SOCKET srvSock);
	void							_doSucess(SOCKET cliSock, SOCKET srvSock);
	void							_doFailed(SOCKET cliSock, SOCKET srvSock, int errCode);
	int								_sendHello(SOCKET srvSock);
	int								_buildHello(char *buffer, size_t bufsize);
	int								_getSockErr(SOCKET s);
	void							_pull(MCONNECT_NODE *pNode, struct list_head *head);

private:
	MHand							m_clHandModule;
	unsigned long					m_hConnHandle;
	MAcceptMgr						m_stAcceptedMgr;
	unsigned int					m_uiGcSIP;		/* 需要每次连接前都要重新设置 且为大端模式*/
	unsigned short					m_usGcSPort;	/* 需要每次连接前都要重新设置 且为大端模式*/
};

#endif