/************************************************************************/
/* �ļ���:                                                              */
/*			core/MHand.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�3��ģ��	��������					*/
/* ����˵��:															*/
/*			GcClient��Ҫ�������̷�Ϊ���								*/
/*			(0)MCoreMgr ģ��	��Ҫ�������							*/
/*			(1)MListen	ģ��	�������								*/
/*			(2)MConnect	ģ��	�����첽connect��channel��				*/
/*			(3)MHand	ģ��	�������ֽ���							*/
/*			(4)MTrans	ģ��	��������ת��							*/
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


/* ��������ӵ��׽��ֶԸ��� */
typedef struct _CONN_LIST_
{
	SOCKET								m_sCliSock;			/** client socket */
	SOCKET								m_sSrvSock;			/** server socket */
	struct list_head					m_lListNode;		/** list node */
} CONN_NODE;


/* ������������ӵ��׽��ֶ� */
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



/* ����״̬����*/
typedef enum
{
	RCVING_HELLO	= 0,		/* ��SRV��HELLO��Ӧ��*/
	RCVING_UNIT_FROM_CLI = 1,	/* ��CLI�����ְ�������SRV�����ְ�*/
	RCVING_UNIT_FROM_SRV = 2,	/* ��SRV���ְ���Ӧ������CLI��Ӧ*/
	TRANSFORMING = 3			/* ����ת��״̬ */
} PEER_STATUS;


typedef struct _HAND_PEER_
{
	SOCKET						m_cliSock;		/** �ͻ����׽���*/
	SOCKET						m_srvSock;		/** ������׽���*/
	PEER_STATUS					m_peerState;	/** �׽��ֶԵ�״̬*/
	struct list_head			m_List;
} HAND_PEER;


/* HAND�߳���ʹ�õľֲ�����������HAND�߳���ʹ��*/
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