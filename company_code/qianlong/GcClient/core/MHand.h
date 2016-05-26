/************************************************************************/
/* 文件名:                                                              */
/*			core/MHand.h												*/
/* 功能:																*/
/*			GcClient主要处理流程的第3层模块	处理握手					*/
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
/*			2013-11-28	卢富波		创建								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_HAND_H_
#define _M_HAND_H_

#include "MTrans.h"


/* 已完成连接的套接字对个体 */
typedef struct _CONN_LIST_
{
	SOCKET								m_sCliSock;			/** client socket */
	SOCKET								m_sSrvSock;			/** server socket */
	struct list_head					m_lListNode;		/** list node */
} CONN_NODE;


/* 管理已完成连接的套接字对 */
class MConnedMgr
{
public:
									MConnedMgr();
									~MConnedMgr();
	
	int								insert(SOCKET cliSock, SOCKET srvSock);
	int								del(SOCKET &cliSock, SOCKET &srvSock);
	void							destroy();

private:
	void							_setSock(SOCKET sock);
	
private:
	CRITICAL_SECTION				m_Lock;				/** lock this struct array*/
	struct list_head				m_stUsed;
	struct list_head				m_stFree;
	CONN_NODE						m_stConnArray[MAX_SOCK_COUNT];	
};



/* 握手状态定义*/
typedef enum
{
	RCVING_HELLO	= 0,		/* 收SRV的HELLO响应包*/
	RCVING_UNIT_FROM_CLI = 1,	/* 收CLI的握手包，并向SRV报握手包*/
	RCVING_UNIT_FROM_SRV = 2,	/* 收SRV握手包响应，并向CLI响应*/
	TRANSFORMING = 3			/* 数据转发状态 */
} PEER_STATUS;


typedef struct _HAND_PEER_
{
	SOCKET						m_cliSock;		/** 客户端套接字*/
	SOCKET						m_srvSock;		/** 服务端套接字*/
	PEER_STATUS					m_peerState;	/** 套接字对的状态*/
	struct list_head			m_List;
} HAND_PEER;


/* HAND线程中使用的局部变量，仅在HAND线程中使用*/
typedef struct _MLOCALHAND_
{
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	HAND_PEER							m_sDataArr[MAX_SOCK_COUNT];
										_MLOCALHAND_();
										~_MLOCALHAND_();
} MLOCALHAND;




class MHand
{
public:
									MHand();
									~MHand();

	int								init();
	void							destroy();
	void							waitExit(int *err, size_t size);
	
	int								insert(SOCKET cliSock, SOCKET srvSock);	
	int								del(SOCKET &cliSock, SOCKET &srvSock);


protected:
	static unsigned int __stdcall	handThread(void * in);

private:
	int								_insertToLocal(MLOCALHAND &local, SOCKET cliSock, SOCKET srvSock, PEER_STATUS state);

	int								_rcvHello(HAND_PEER *peer);
	int								_doRcv(HAND_PEER *peer, bool isSrv);
	int								_rcvUnitFromSrv(HAND_PEER *peer);
	int								_rcvUnitFromCli(HAND_PEER *peer);
	void							_doFailed(MLOCALHAND &localHand, HAND_PEER *peer, int errCode);
	void							_doSucess(MLOCALHAND &localHand, HAND_PEER *peer);
	void							_pull(HAND_PEER *pNode, struct list_head *head);
	char*							_StrStr(char* pBuf, int nSize, char* pSearch);
	

private:
	MTrans							m_clTranModule;
	unsigned long					m_hHandHandle;
	MConnedMgr						m_clConnetedMgr;
};



#endif