/************************************************************************/
/* 文件名:                                                              */
/*			core/MTrans.h												*/
/* 功能:																*/
/*			GcClient主要处理流程的第3层模块	即数据转发模块				*/
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
#ifndef _M_TRANS_H_
#define _M_TRANS_H_

#include <windows.h>
#include <process.h>
#include "MBaseFunc.h"
#include "error.h"
#include "list.h"
#include "MBaseConfig.h"
#include "MHttpParase.h"





/* 定义已完成握手协议的套接字对，仅在TRANS线程中使用 */
typedef struct _HANDED_NODE_
{
	SOCKET					m_sCliSock;	
	SOCKET					m_sSrvSock;
	struct list_head		m_lListNode;
} HANDED_NODE;

/* 定义管理已完成握手协议的套接字对，作为TRANS线程的私有数据 */
class MHandedMgr
{
public:
										MHandedMgr();
										~MHandedMgr();

	void								destroy();
	void								delHanded();
	int									insert(SOCKET cliSock, SOCKET srvSock);
	int									del(SOCKET &cliSock, SOCKET &srvSock);
private:
	CRITICAL_SECTION					m_Lock;				/** lock this struct array*/
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	HANDED_NODE							m_stHandedData[MAX_SOCK_COUNT];
};

/* 定义数据转发时用到的套接字属性，仅在TRANS_SOCKET_PEER有用 */ 
typedef struct _SOCKET_INFO_
{
	SOCKET								m_sock;	
	char								m_sockBuffer[MAX_HTTPHEAD_SIZE+MAX_IOBUFFER_LEN+4];/* 缓冲区大小,+4是由于要加"\r\n\r\n" */
	unsigned short						m_UnitSerial;				/** 组装数据要用的unit*/
	unsigned short						m_pad;						/** 保留*/			
	unsigned long						m_ulLinkNo;					/** 组装数据要用到的link*/
	HTTP_PARASE_ENG						m_engine;					/** http解析引擎,仅对SRV有用*/
	unsigned long						m_tranSize;					/* 转发的纯数据大小*/
}SOCKET_INFO;

/* 定义数据转发用到的套接字对，仅在MLOCALTRANS有用 */
typedef struct _TRANS_SOCKET_PEER_
{
	SOCKET_INFO							m_cli;
	SOCKET_INFO							m_srv;
	struct list_head					m_lListNode;
}TRANS_SOCKET_PEER;


/** 定义TRANS线程中使用的局部变量，仅在TRANS线程中使用*/
typedef struct _MLOCALTRANS_
{
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	TRANS_SOCKET_PEER					m_sDataArr[MAX_SOCK_COUNT];

										_MLOCALTRANS_();
										~_MLOCALTRANS_();
	void								initTransNode(TRANS_SOCKET_PEER *Node);
	
} MLOCALTRANS;



class MTrans
{
public:
										MTrans();
										~MTrans();

	int									init();
	void								destroy();
	void								waitExit(int *err, size_t size);
	int									insert(SOCKET cliSock, SOCKET srvSock);	
	int									del(SOCKET &cliSock, SOCKET &srvSock);


protected:
	static unsigned int __stdcall		transThread(void * in);

private:
	int									_insertToLocal(MLOCALTRANS &local,SOCKET cliSock, SOCKET srvSock);
	int									_trans(MLOCALTRANS &local, TRANS_SOCKET_PEER *pNode, bool isSrv);
	int									_cliToSrv(TRANS_SOCKET_PEER* peer);
	int									_srvToCli(TRANS_SOCKET_PEER* peer);

private:
	unsigned long						m_hTraHandle;
	MHandedMgr							m_clHandedMgr;
};



#endif