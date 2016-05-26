/************************************************************************/
/* �ļ���:                                                              */
/*			core/MTrans.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�3��ģ��	������ת��ģ��				*/
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
#ifndef _M_TRANS_H_
#define _M_TRANS_H_

#include <windows.h>
#include <process.h>
#include "MBaseFunc.h"
#include "error.h"
#include "list.h"
#include "MBaseConfig.h"
#include "MHttpParase.h"





/* �������������Э����׽��ֶԣ�����TRANS�߳���ʹ�� */
typedef struct _HANDED_NODE_
{
	SOCKET					m_sCliSock;	
	SOCKET					m_sSrvSock;
	struct list_head		m_lListNode;
} HANDED_NODE;

/* ����������������Э����׽��ֶԣ���ΪTRANS�̵߳�˽������ */
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

/* ��������ת��ʱ�õ����׽������ԣ�����TRANS_SOCKET_PEER���� */ 
typedef struct _SOCKET_INFO_
{
	SOCKET								m_sock;	
	char								m_sockBuffer[MAX_HTTPHEAD_SIZE+MAX_IOBUFFER_LEN+4];/* ��������С,+4������Ҫ��"\r\n\r\n" */
	unsigned short						m_UnitSerial;				/** ��װ����Ҫ�õ�unit*/
	unsigned short						m_pad;						/** ����*/			
	unsigned long						m_ulLinkNo;					/** ��װ����Ҫ�õ���link*/
	HTTP_PARASE_ENG						m_engine;					/** http��������,����SRV����*/
	unsigned long						m_tranSize;					/* ת���Ĵ����ݴ�С*/
}SOCKET_INFO;

/* ��������ת���õ����׽��ֶԣ�����MLOCALTRANS���� */
typedef struct _TRANS_SOCKET_PEER_
{
	SOCKET_INFO							m_cli;
	SOCKET_INFO							m_srv;
	struct list_head					m_lListNode;
}TRANS_SOCKET_PEER;


/** ����TRANS�߳���ʹ�õľֲ�����������TRANS�߳���ʹ��*/
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