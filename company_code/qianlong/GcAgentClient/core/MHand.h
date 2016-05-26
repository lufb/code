/************************************************************************/
/* �ļ���:                                                              */
/*			core/MHand.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�3��ģ��	��������					*/
/* ����˵��:															*/
/*			GcClient��Ҫ�������̷�Ϊ�߲�								*/
/*			(0)MCoreMgr ģ��	��Ҫ������ڼ�����ģ��					*/
/*			(1)MListen	ģ��	�������								*/
/*			(2)MRcvOpen	ģ��	�������û���Э����Ϣ,����Э���������	*/
/*			(3)MCONN	ģ��	�����첽����GcS(���ߴ��������)			*/
/*			(4)MAGENT	ģ��	����������֤(����û�ͨ����������)	*/
/*			(5)MHAND	ģ��	������GcS���ֽ���						*/
/*			(6)MTrans	ģ��	��������ת��							*/
/*			�����²�ģ�����ֱ���ϲ�ģ�����							*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-28	¬����		����								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_HAND_H_
#define _M_HAND_H_

#include "MTrans.h"

/* ����ɴ���ĸ��� */
typedef struct _CONN_
{
	SOCKET								m_sCliSock;			/* client socket */
	SOCKET								m_sSrvSock;			/* server socket */
	char								m_cDstIP[256];		/* dst ip*/
	unsigned char						m_cApplyType;		/* type�����ж��Ƿ�Ϊί�� */
	unsigned short						m_usDstPort;		/* dst port*/

	void								init();
} AGENT_NODE;

/* ����ɴ���ĸ��������ڵ� */
typedef struct _CONN_LIST_
{
	AGENT_NODE								m_stAgeNode;			
	struct list_head						m_listNode;
} AGENT_LIST_NODE;


/* �����������ɴ������ */
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



/* ����״̬����*/
typedef enum
{
	HAND_CONN_ED					= 0,	/* ���������(��ʼ��״̬) */
	HAND_SEND_HELLO_ED				= 1,	/* �ѷ���HELLO�� */
	HAND_RECV_HELLO_ED				= 2,	/* �������HELLO�� */
	HAND_SEND_OPEN_ED				= 3,	/* �ѷ���OPEN�� */
	HAND_RECV_OPEN_ED				= 4,	/* �������OPEN�����·����ͻ���,���������*/
} PEER_STATUS;


typedef struct _HAND_PEER_
{
	AGENT_NODE					m_stAgentedNode;
	PEER_STATUS					m_peerState;	/** �׽��ֶԵ�״̬*/
	struct list_head			m_listNode;
} HAND_LIST_NODE;


/* HAND�߳���ʹ�õľֲ�����������HAND�߳���ʹ��*/
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