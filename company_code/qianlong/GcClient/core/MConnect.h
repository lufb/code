/************************************************************************/
/* �ļ���:                                                              */
/*			core/MConnect.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�2��ģ��	���첽����ģ��				*/
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
/*			2013-11-25	¬����		����								*/
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

/* �������accept��ɺ���׽���  */
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


/** CONNECT�߳������CONNECT�Ľڵ���Ϣ������CONNECT�߳���ʹ��*/
typedef struct _MCONNECT_NODE_
{
	SOCKET								m_sCliSock;			/** client socket */
	SOCKET								m_sSrvSock;			/** server socket */
	struct list_head					m_lListNode;		/** list node */
} MCONNECT_NODE;

/** CONNECT�߳���ʹ�õľֲ�����������CONNECT�߳���ʹ��*/
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
	//����˽�к�����ֻ������CONN�߳��е���
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
	unsigned int					m_uiGcSIP;		/* ��Ҫÿ������ǰ��Ҫ�������� ��Ϊ���ģʽ*/
	unsigned short					m_usGcSPort;	/* ��Ҫÿ������ǰ��Ҫ�������� ��Ϊ���ģʽ*/
};

#endif