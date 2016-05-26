/************************************************************************/
/* �ļ���:                                                              */
/*			core/MConnect.h												*/
/* ����:																*/
/*			GcClient��Ҫ�������̵ĵ�2��ģ��	���첽����ģ��				*/
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
#include "MAgent.h"


/* ��������������Open������ */
class MRcvedOpenMgr
{
public:
										MRcvedOpenMgr();
										~MRcvedOpenMgr();

public:
	int									insert(GC_PRO &gcPro);
	int									del(GC_PRO &gcPro);
	void								destroy();
	
	
private:
	CRITICAL_SECTION					m_Lock;				/** lock this struct array*/
	struct list_head					m_stUsed;			/** list used sock that accept's*/
	struct list_head					m_stFree;			/** list free sock that can accept's*/
	RCVED_OPEN_NODE						m_stRcvOpenArr[MAX_SOCK_COUNT];
};



/** CONNECT�߳���ʹ�õľֲ�����������CONNECT�߳���ʹ��*/
typedef struct _MLOCALCONNECT_
{
	struct list_head					m_stUsed;
	struct list_head					m_stFree;
	MCONNECT_NODE						m_sDataArr[MAX_SOCK_COUNT];

										_MLOCALCONNECT_();
 										~_MLOCALCONNECT_();

	int									insert(GC_PRO &gcPro);
	void								del(MCONNECT_NODE *pNode);
	int									isFull();
	int									isEmpty();
} MLOCALCONNECT;



class MConnect
{
public:
									MConnect();
									~MConnect();

	int								init();
	void							destroy();
	void							waitExit(int *err, size_t size);
	int								insert(GC_PRO &gcPro);
	

protected:
	static unsigned int __stdcall	connThread(void * in);

	//����˽�к�����ֻ������CONN�߳��е���
private:
	int								_del(GC_PRO &gcPro);	
	void							_doSucess(GC_PRO &gcPro);
	void							_doFailed(GC_PRO &gcPro, int errCode);
	int								_getSockErr(SOCKET s);
	int								_upConnSrv( GC_PRO &gcPro);
	int								_getConnSrv(const GC_PRO &gcPro);

private:
	MAgent							m_clAgeModule;
	unsigned long					m_hConnHandle;
	MRcvedOpenMgr					m_stRcvedOpenMgr;
	unsigned int					m_uiConnIP;		/* ��Ҫÿ������ǰ��Ҫ�������� ��Ϊ���ģʽ*/
	unsigned short					m_usConnPort;	/* ��Ҫÿ������ǰ��Ҫ�������� ��Ϊ���ģʽ*/
};

#endif