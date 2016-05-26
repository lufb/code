#include <memory.h>
#include "MAgeComm.h"
#include "MLocalLock.h"
#include "error.h"
#include "MGlobal.h"
#include "MList.h"
#include "MSocket.h"
#include "gcCliInterface.h"


/**
 *	_GC_PRO_		 -		���캯��
 *
 *	
 *	return
 *						��
 */
_GC_PRO_::_GC_PRO_()
{
	init();
}

/**
 *	init		 -		��ʼ���ڵ�
 *
 *	
 *	return
 *						��
 */
void _GC_PRO_::init()
{
	m_eType = INVALID_REG;
	m_bNeedAuth = false;
	m_ucApplyType = (unsigned char)1;	/*	Ĭ��Ϊ��ί�� */
	memset(m_cUrName, 0, sizeof(m_cUrName));
	memset(m_cPassWd, 0, sizeof(m_cPassWd));
	memset(m_cAgIP, 0, sizeof(m_cAgIP));
	memset(m_cDstIP, 0, sizeof(m_cDstIP));
	m_usAgPort = -1;
	m_usDstPort = -1;
	m_sCliSock = INVALID_SOCKET;
	m_sSrvSock = INVALID_SOCKET;
	memset(m_cGcSIP, 0, sizeof(m_cGcSIP));
	m_usGcSPort = -1;
}

//////////////////////////////////////////////////////////////////////////

MConnectedMgr::MConnectedMgr()
{
	INIT_LIST_HEAD(&m_stFree);
	INIT_LIST_HEAD(&m_stUsed);
	InitializeCriticalSection(&m_Lock);
	
	/** init */
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_stConnedArr[i].m_stGcPro.init();
		list_add_tail(&(m_stConnedArr[i].m_listNode), &m_stFree);
	}
}

MConnectedMgr::~MConnectedMgr()
{
	DeleteCriticalSection(&m_Lock);
}

/**
 *	destroy		-				�ͷ���Դ,���ü���(�Ѿ�û�����������)
 *
 *
 *	return
 *								��
 */
void MConnectedMgr::destroy()
{
	struct list_head				*pList, *pSafe;
	CONNED_NODE						*pNode;
	int								total = 0;
	
	list_for_each_safe(pList, pSafe, &m_stUsed)
	{
		++total;
		pNode = list_entry(pList, CONNED_NODE, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��CONNED�����йر��׽��ֶ�[%d:%d]",
			pNode->m_stGcPro.m_sCliSock, pNode->m_stGcPro.m_sSrvSock);
		MSocket::close(pNode->m_stGcPro.m_sCliSock);
		MSocket::close(pNode->m_stGcPro.m_sSrvSock);

		pNode->m_stGcPro.m_sCliSock = INVALID_SOCKET;
		pNode->m_stGcPro.m_sSrvSock = INVALID_SOCKET;

		list_del(pList);
		list_add_tail(pList, &m_stFree);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��CONNED�����й�ɾ��[%d]���׽��ֶ�",
		total);
	}	
}

/**
 *	insert		-				�������
 *
 *	@gcPro:			[in]		��Ҫ��ӵ�����
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	MConnectedMgr::insert(const GC_PRO &gcPro)
{
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);
	
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, EHANDFULL);
	
	/** fill data*/
	CONNED_NODE	*pNode = list_entry(m_stFree.next, CONNED_NODE, m_listNode);
	memcpy(&(pNode->m_stGcPro), &gcPro, sizeof(GC_PRO));
	
// 	Global_Log.writeLog(LOG_TYPE_INFO,
// 		"�׽��ֶ�[%d:%d]�������",
// 		gcPro.m_sCliSock, gcPro.m_sSrvSock);
	
	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
}

/**
 *	del		-					ȡԪ��
 *
 *	@gcPro		[in/out]		ȡ����Ԫ��
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MConnectedMgr::del(GC_PRO &gcPro)
{
	MLocalSection					locSec;							
	
	gcPro.init();
	
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** �����*/
		return BUILD_ERROR(0, ELISTEMPTY);
	
	CONNED_NODE	*pNode = list_entry(m_stUsed.next, CONNED_NODE, m_listNode);
	memcpy(&gcPro, &(pNode->m_stGcPro), sizeof(GC_PRO));
	
	/* ��Դ�ع�ʱ���³�ʼ�� */
	pNode->m_stGcPro.init();
	return MList::_del2insert(&(pNode->m_listNode), &m_stFree);
}

/**
 *	sndErrToCli		-			��ͻ��˷��ش�����֤�������
 *
 *	@err		[in/out]		ȡ����Ԫ��
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
void sndErrToCli(const int err, const SOCKET cliSock)
{
	char					sndBuffer[128];

	tagQLGCProxy_Apply * pApplay = (tagQLGCProxy_Apply *)sndBuffer;
	tagQLGCProxy_CertifyRet * pCertify = (tagQLGCProxy_CertifyRet *)(sndBuffer + sizeof(tagQLGCProxy_Apply));
	
	memcpy(pApplay->szFlag, "QLGC", 4);
	pApplay->ucApplyType = 1;
	pApplay->usSize = sizeof(tagQLGCProxy_CertifyRet);
	pCertify->ucReply = err;	/* ������֤���������صĴ����� */
	pCertify->CheckCode = '!';
	
	MSocket::sendDataNonBlock(cliSock, sndBuffer,
		sizeof(tagQLGCProxy_Apply)+sizeof(tagQLGCProxy_CertifyRet));
}