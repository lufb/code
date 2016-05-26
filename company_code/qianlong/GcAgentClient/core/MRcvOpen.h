/************************************************************************/
/* �ļ���:                                                              */
/*			core/MRcvOpen.h												*/
/* ����:																*/
/*			�������û���Э����Ϣ,����Э���������						*/
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
/*			2013-12-20	¬����		����								*/
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

/* �������accept��ɺ���׽���  */
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


/** �ڵ���Ϣ������RCVOPEN�߳���ʹ��*/
typedef struct _MRCVOPEN_NODE_
{
	SOCKET								m_sSock;			/** socket(client sock) */
	struct list_head					m_lListNode;		/** list node */
} MRCVOPEN_NODE;


/** RCVOPEN�߳���ʹ�õľֲ�����������RCVOPEN�߳���ʹ��*/
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
	MAcceptMgr							m_stAcceptedMgr;	/* ����accept����׽��� */
	MConnect							m_clConnModule;		/* �²�ģ��(����ģ��)*/
	unsigned long						m_hRcvOpenHandle;	/*	�����߳̾��*/
};





#endif