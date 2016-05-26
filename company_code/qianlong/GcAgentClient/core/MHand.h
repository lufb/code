/************************************************************************/
/* 文件名:                                                              */
/*			core/MHand.h												*/
/* 功能:																*/
/*			GcClient主要处理流程的第3层模块	处理握手					*/
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
/*			2013-11-28	卢富波		创建								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_HAND_H_
#define _M_HAND_H_

#include "MTrans.h"

/* 已完成代理的个体 */
typedef struct _CONN_
{
	SOCKET								m_sCliSock;			/* client socket */
	SOCKET								m_sSrvSock;			/* server socket */
	char								m_cDstIP[256];		/* dst ip*/
	unsigned char						m_cApplyType;		/* type用以判断是否为委托 */
	unsigned short						m_usDstPort;		/* dst port*/

	void								init();
} AGENT_NODE;

/* 已完成代理的个体的链表节点 */
typedef struct _CONN_LIST_
{
	AGENT_NODE								m_stAgeNode;			
	struct list_head						m_listNode;
} AGENT_LIST_NODE;


/* 定义管理已完成代理的类 */
class MAgentedMgr
{
public:
									MAgentedMgr();
									~MAgentedMgr();
	
	int								insert(const SOCKET cliSock, const SOCKET srvSock, const char *dstIP, const unsigned short dstPort, const unsigned char applyType);
	int								del(AGENT_NODE	&ageNode);
	void							destroy();
	
private:
	CRITICAL_SECTION				m_Lock;				/** lock this struct array*/
	struct list_head				m_stUsed;
	struct list_head				m_stFree;
	AGENT_LIST_NODE					m_stAgentedArr[MAX_SOCK_COUNT];	
};



/* 握手状态定义*/
typedef enum
{
	HAND_CONN_ED					= 0,	/* 已完成连接(初始化状态) */
	HAND_SEND_HELLO_ED				= 1,	/* 已发送HELLO包 */
	HAND_RECV_HELLO_ED				= 2,	/* 已完成收HELLO包 */
	HAND_SEND_OPEN_ED				= 3,	/* 已发送OPEN包 */
	HAND_RECV_OPEN_ED				= 4,	/* 已完成收OPEN包和下发给客户端,即握手完成*/
} PEER_STATUS;


typedef struct _HAND_PEER_
{
	AGENT_NODE					m_stAgentedNode;
	PEER_STATUS					m_peerState;	/** 套接字对的状态*/
	struct list_head			m_listNode;
} HAND_LIST_NODE;


/* HAND线程中使用的局部变量，仅在HAND线程中使用*/
typedef struct _MLOCALHAND_
{
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	HAND_LIST_NODE						m_sDataArr[MAX_SOCK_COUNT];


										_MLOCALHAND_();
										~_MLOCALHAND_();

	int									isFull();
	int									isEmpty();
	int									insert(AGENT_NODE &agentNode, PEER_STATUS state);
	void								del(HAND_LIST_NODE *pNode);
} MLOCALHAND;


class MHand
{
public:
									MHand();
									~MHand();

	int								init();
	void							destroy();
	void							waitExit(int *err, size_t size);
	
	int								insert(const SOCKET cliSock, const SOCKET srvSock, const char *dstIP, 
										const unsigned short dstPort, const unsigned char applyType);	
protected:
	static unsigned int __stdcall	handThread(void * in);

private:
	int								_buildHello(char *buffer, size_t bufsize);
	int								_sndHello(AGENT_NODE agentedNode);
	int								_rcvHello(HAND_LIST_NODE *peer);
	int								_doRcv(HAND_LIST_NODE *peer);
	int								_sndOpen(HAND_LIST_NODE *peer);
	int								_rcvOpen(HAND_LIST_NODE *peer);
	void							_doFailed(AGENT_NODE &ageNode, int errCode, PEER_STATUS state);
	void							_doSucess(HAND_LIST_NODE *peer);
	char*							_StrStr(char* pBuf, int nSize, char* pSearch);
	
private:
	MTrans							m_clTranModule;
	unsigned long					m_hHandHandle;
	MAgentedMgr						m_clAgentedMgr;
};



#endif