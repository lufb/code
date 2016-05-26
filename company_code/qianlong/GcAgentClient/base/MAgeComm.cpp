#include <memory.h>
#include "MAgeComm.h"
#include "MLocalLock.h"
#include "error.h"
#include "MGlobal.h"
#include "MList.h"
#include "MSocket.h"
#include "gcCliInterface.h"


/**
 *	_GC_PRO_		 -		构造函数
 *
 *	
 *	return
 *						无
 */
_GC_PRO_::_GC_PRO_()
{
	init();
}

/**
 *	init		 -		初始化节点
 *
 *	
 *	return
 *						无
 */
void _GC_PRO_::init()
{
	m_eType = INVALID_REG;
	m_bNeedAuth = false;
	m_ucApplyType = (unsigned char)1;	/*	默认为非委托 */
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
 *	destroy		-				释放资源,不用加锁(已经没人在用这个了)
 *
 *
 *	return
 *								无
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
			"从CONNED队列中关闭套接字对[%d:%d]",
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
			"从CONNED队列中共删除[%d]个套接字对",
		total);
	}	
}

/**
 *	insert		-				添加数据
 *
 *	@gcPro:			[in]		需要添加的数据
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
// 		"套接字对[%d:%d]完成连接",
// 		gcPro.m_sCliSock, gcPro.m_sSrvSock);
	
	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
}

/**
 *	del		-					取元素
 *
 *	@gcPro		[in/out]		取出的元素
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MConnectedMgr::del(GC_PRO &gcPro)
{
	MLocalSection					locSec;							
	
	gcPro.init();
	
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** 链表空*/
		return BUILD_ERROR(0, ELISTEMPTY);
	
	CONNED_NODE	*pNode = list_entry(m_stUsed.next, CONNED_NODE, m_listNode);
	memcpy(&gcPro, &(pNode->m_stGcPro), sizeof(GC_PRO));
	
	/* 资源回归时重新初始化 */
	pNode->m_stGcPro.init();
	return MList::_del2insert(&(pNode->m_listNode), &m_stFree);
}

/**
 *	sndErrToCli		-			向客户端返回代理认证出错代码
 *
 *	@err		[in/out]		取出的元素
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
void sndErrToCli(const int err, const SOCKET cliSock)
{
	char					sndBuffer[128];

	tagQLGCProxy_Apply * pApplay = (tagQLGCProxy_Apply *)sndBuffer;
	tagQLGCProxy_CertifyRet * pCertify = (tagQLGCProxy_CertifyRet *)(sndBuffer + sizeof(tagQLGCProxy_Apply));
	
	memcpy(pApplay->szFlag, "QLGC", 4);
	pApplay->ucApplyType = 1;
	pApplay->usSize = sizeof(tagQLGCProxy_CertifyRet);
	pCertify->ucReply = err;	/* 带的认证服务器返回的错误码 */
	pCertify->CheckCode = '!';
	
	MSocket::sendDataNonBlock(cliSock, sndBuffer,
		sizeof(tagQLGCProxy_Apply)+sizeof(tagQLGCProxy_CertifyRet));
}