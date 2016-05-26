#include <assert.h>
#include "MTrans.h"
#include "MLocalLock.h"
#include "MList.h"
#include "MGlobal.h"
#include "MHttpParase.h"
#include "MSocket.h"

/*
 *	_MLOCALTRANS_	-	���캯�������߳��д����þֲ�����ʱ�ĳ�ʼ��
 */
_MLOCALTRANS_::_MLOCALTRANS_()
{
	INIT_LIST_HEAD(&m_stUsed);
	INIT_LIST_HEAD(&m_stFree);
	for( int i = 0; i < MAX_SOCK_COUNT; ++i )
	{
		initTransNode(m_sDataArr + i);	
		list_add_tail(&(m_sDataArr[i].m_lListNode), &m_stFree);
	}
}

/**
 *	initTransNode		-		��ʼ��һ��TRANS_SOCKET_PEER�ڵ�
 *
 *	@pNode			[in/out]	��Ҫ��ʼ���Ľڵ�
 *
 *
 *	return
 *								��
 */
void _MLOCALTRANS_::initTransNode(TRANS_SOCKET_PEER *pNode)
{
	assert(pNode != NULL);
	MHttpParase::init_parase_eng(pNode->m_cli.m_engine);
	pNode->m_cli.m_sock = INVALID_SOCKET;
	pNode->m_cli.m_UnitSerial = 0;
	pNode->m_cli.m_ulLinkNo = 0;
	pNode->m_cli.m_tranSize = 0;

	MHttpParase::init_parase_eng(pNode->m_srv.m_engine);
	pNode->m_srv.m_sock = INVALID_SOCKET;
	pNode->m_srv.m_UnitSerial = 0;
	pNode->m_srv.m_ulLinkNo = 0;
	pNode->m_srv.m_tranSize = 0;
}

/*
 *	~_MLOCALTRANS_	-	�������������߳��˳���ر������׽���
 */
_MLOCALTRANS_::~_MLOCALTRANS_()
{
	struct list_head		*pTmp;
	TRANS_SOCKET_PEER		*pNode;
	int						total = 0;/* �洢�������׽��ֶԣ�����鿴���� */

	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, TRANS_SOCKET_PEER, m_lListNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"TRANS�߳��˳������ر��׽��ֶ�[%d:%d],��ת���ֽ���[%d:%d], �����ֽ���[%d:%d]",
			pNode->m_cli.m_sock, pNode->m_srv.m_sock,
			pNode->m_cli.m_tranSize, pNode->m_srv.m_tranSize,
			pNode->m_cli.m_engine.leaveParased, pNode->m_srv.m_engine.leaveParased);
		MSocket::close(pNode->m_cli.m_sock);
		MSocket::close(pNode->m_srv.m_sock);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"TRANS�߳��˳��󣬹��ͷ�[%d]���׽��ֶ�",
		total);
	}	
}

/**
 *	insert		-				�������
 *
 *	@cliSock:		[in]		�ͻ����׽���
 *	@srvSock:		[in]		������׽���
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	_MLOCALTRANS_::insert(SOCKET cliSock, SOCKET srvSock)
{
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ELINKOVERFLOW);
	
	TRANS_SOCKET_PEER	*pNode = list_entry(m_stFree.next, TRANS_SOCKET_PEER, m_lListNode);
	/* ���ó�ʼ���ڵ���Ϣ:��free�õ��Ľڵ㱣֤�Ǿ�����ʼ���� */
	/* ֻ��ֱ����������׽��� */
	pNode->m_cli.m_sock = cliSock;
	pNode->m_srv.m_sock = srvSock;
	
	/* ���ýڵ��m_stFree�ӵ�m_stUsed������ */
	return MList::_del2insert(&(pNode->m_lListNode), &m_stUsed);
}

/**
 *	del		-					ɾ�����ݣ�������Ӧ��ʼ��
 *
 *	@pNode			[in]		��Ҫɾ���Ľڵ�
 *
 *
 *	return
 *								��
 */
void _MLOCALTRANS_::del(TRANS_SOCKET_PEER *pNode)
{
	/* ���³�ʼ���ýڵ� */
	initTransNode(pNode);

	/* ��Դ�ع� */
	list_del(&(pNode->m_lListNode));
	list_add_tail(&(pNode->m_lListNode), &m_stFree);
}
/**
 *	isFull		-				�Ƿ���
 *
 *
 *	Note:						ֻ�ڸ�ģ����߳���ʹ�ã����ü���
 *
 *	return
 *		!=0						����
 *		==0						û��
 */
int _MLOCALTRANS_::isFull()
{
	return list_empty(&m_stFree);
}

/**
 *	isEmpty		-				�Ƿ��ǿ�
 *
 *
 *	Note:						ֻ�ڸ�ģ����߳���ʹ�ã����ü���
 *
 *	return
 *		!=0						�ѿ�
 *		==0						û��
 */
int	_MLOCALTRANS_::isEmpty()
{
	return list_empty(&m_stUsed);
}

//////////////////////////////////////////////////////////////////////////

MHandedMgr::MHandedMgr()
{
	INIT_LIST_HEAD(&m_stFree);
	INIT_LIST_HEAD(&m_stUsed);
	InitializeCriticalSection(&m_Lock);
	
	/** init */
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_stHandedData[i].m_sCliSock = INVALID_SOCKET;
		m_stHandedData[i].m_sSrvSock = INVALID_SOCKET;
		list_add_tail(&(m_stHandedData[i].m_lListNode), &m_stFree);
	}
}

MHandedMgr::~MHandedMgr()
{
	DeleteCriticalSection(&m_Lock);
}


/**
 *	delHanded		-			���handed����
 *
 *
 *	return
 *								��
 */
void MHandedMgr::delHanded()
{
	struct list_head				*pList, *pSafe;
	HANDED_NODE						*pNode;
	int								total = 0;
	MLocalSection					local;
	
	local.Attch(&m_Lock);	
	list_for_each_safe(pList, pSafe, &m_stUsed)
	{
		++total;
		pNode = list_entry(pList, HANDED_NODE, m_lListNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��handed�����йر��׽��ֶ�[%d,%d]",
			pNode->m_sCliSock, pNode->m_sSrvSock);

		MSocket::close(pNode->m_sCliSock);
		MSocket::close(pNode->m_sSrvSock);

		pNode->m_sCliSock = INVALID_SOCKET;
		pNode->m_sSrvSock = INVALID_SOCKET;
		
		list_del(pList);
		list_add_tail(pList, &m_stFree);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��handed�����й�ɾ��[%d]���׽���",
		total);
	}
}

/**
 *	destroy		-				�ͷ���Դ,���ü���(�Ѿ�û�����������)
 *
 *
 *	return
 *								��
 */
void MHandedMgr::destroy()
{
	delHanded();
}
/**
 *	insert		-		���Ԫ��
 *
 *	@cliSock					�ͻ����׽���
 *	@srvSock					������׽���
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MHandedMgr::insert(SOCKET cliSock, SOCKET srvSock)
{	
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);

	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, EHANDFULL);
	
	/** fill sock*/
	HANDED_NODE	*pNode = list_entry(m_stFree.next, HANDED_NODE, m_lListNode);
	pNode->m_sCliSock = cliSock;
	pNode->m_sSrvSock = srvSock;

	Global_Log.writeLog(LOG_TYPE_INFO,
		"�׽��ֶ�[%d:%d]�������",
		cliSock, srvSock);
	
	return MList::_del2insert(&(pNode->m_lListNode), &m_stUsed);
}

/**
 *	del		-					ȡԪ��
 *
 *	@node						ȡ����Ԫ��
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MHandedMgr::del(SOCKET &cliSock, SOCKET &srvSock)
{
	MLocalSection					locSec;							
	
	cliSock = INVALID_SOCKET;
	srvSock = INVALID_SOCKET;
	
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** �����*/
		return BUILD_ERROR(0, ELISTEMPTY);
	
	HANDED_NODE	*pNode = list_entry(m_stUsed.next, HANDED_NODE, m_lListNode);
	cliSock = pNode->m_sCliSock;
	srvSock = pNode->m_sSrvSock;
	
	/* ��Դ�ع�ǰ�ĳ�ʼ�� */
	pNode->m_sCliSock = INVALID_SOCKET;
	pNode->m_sSrvSock = INVALID_SOCKET;
	return MList::_del2insert(&(pNode->m_lListNode), &m_stFree);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
MTrans::MTrans()
{
	m_hTraHandle = 0;
}

MTrans::~MTrans()
{

}

int MTrans::init()
{
	m_hTraHandle =  _beginthreadex(NULL, 0, transThread, this, 0, NULL);
	if(m_hTraHandle == -1)
		return BUILD_ERROR(_OSerrno(), ECRETHREAD);

	return 0;
}

/**
 *	destroy		-				�ͷű�����Դ
 *
 *
 *	return
 *								��
 */
void MTrans::destroy()
{
	m_clHandedMgr.destroy();
}


/**
 *	waitExit		-		�ͷű�����Դ���ȴ��²��˳�
 *	
 *	Note:
 *		ÿ���˳��붨�壺
 *					== 0	�ɹ�
 *					!= 0	ʧ��
 *
 *	@err					�洢�Ӹò㵽�������в��˳����˳���
 *	@size					err�Ĵ�С(int�ĸ���)
 *
 *	return
 *							��
 */
void	MTrans::waitExit(int *err, size_t size)
{
	int					rc = 0;

	if(m_hTraHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_hTraHandle, WAIT_THREAD_TIMEOUT);
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�TRANS�߳��˳���ʱ[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�TRANS�߳��˳�����[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}
		m_hTraHandle = 0;
	}

	/* �ͷű�����Դ */
	destroy();

	/* ��䱾������� */
	err[0] = rc;

	/* �����²� */
}

/**
 *	insert		-			���Ԫ��
 *
 *	@cliSock					�ͻ����׽���
 *	@srvSock					������׽���
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MTrans::insert(SOCKET cliSock, SOCKET srvSock)
{
	return m_clHandedMgr.insert(cliSock, srvSock);
}

/**
 *	del		-					ȡԪ��
 *
 *	@node						ȡ����Ԫ��
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MTrans::del(SOCKET &cliSock, SOCKET &srvSock)
{
	return m_clHandedMgr.del(cliSock, srvSock);
}

unsigned int __stdcall MTrans::transThread(void * in)
{
	assert(NULL != in);
	MTrans					*pObj = (MTrans*)in;
	MLOCALTRANS				local;				/* �ֲ�����,����ֲ�������΢�е��Ӧ���ڴ����߳�ʱ����ջ���Щ*/
	fd_set					fdRead;
	SOCKET					cliSock, srvSock;
	struct timeval			tv;
	struct list_head		*pTmp, *pSafe;
	TRANS_SOCKET_PEER		*pNode;
	int						rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);

	do{
		/** ����trans����û��*/
		if(!local.isFull())
		{
			/* ��ͼ��һ���Ѿ�������ɵ��׽��ֶ�*/
			if(pObj->m_clHandedMgr.del(cliSock, srvSock) == 0){
				assert(cliSock != INVALID_SOCKET && srvSock != INVALID_SOCKET);
				local.insert(cliSock, srvSock);		/* ��ӿ϶��ǳɹ��ģ�����У�鷵��ֵ */
			}
		}else{
			Global_Log.writeLog(LOG_TYPE_WARN,
				"TRANS�߳��У��׽��ֶԻ�������������ʼ�ر�handed����");
			pObj->m_clHandedMgr.delHanded();
		}

		if(!local.isEmpty())
		{
			/* ����Ҫ�����׽��� */
			FD_ZERO(&fdRead);
			/* ���������������selectԪ�� */
			list_for_each(pTmp, &local.m_stUsed)
			{
				pNode = list_entry(pTmp, TRANS_SOCKET_PEER, m_lListNode);
				assert(pNode != NULL && pNode->m_cli.m_sock != INVALID_SOCKET && pNode->m_srv.m_sock != INVALID_SOCKET);
				/* ���׽��ֶԼ����� */
				FD_SET(pNode->m_cli.m_sock, &fdRead);
				FD_SET(pNode->m_srv.m_sock, &fdRead);
			}

			rc = select(0, &fdRead, NULL, NULL, &tv);
			if(rc > 0){ /* ��⵽�׽��� */
				list_for_each_safe(pTmp, pSafe, &local.m_stUsed)
				{
 					pNode = list_entry(pTmp, TRANS_SOCKET_PEER, m_lListNode);
					if(FD_ISSET(pNode->m_cli.m_sock, &fdRead))	/* CLI -> SRV */
						if(pObj->_trans(local, pNode, false) != 0)
							continue;	/* ��ǰ��ֹ */
					if(FD_ISSET(pNode->m_srv.m_sock, &fdRead))
						pObj->_trans(local, pNode, true); /* SRV -> CLI */
				}
			}else{
				if(rc < 0){ /* �������� */
					Global_Log.writeLog(LOG_TYPE_ERROR,
					"TRANS�߳���select��������[%d]",
					BUILD_ERROR(_OSerrno(), EABORT));
					assert(0);
				}
			}
		}else{Sleep(15);} /* ��ֹCPU���� */
	}while(Global_IsRun);

	/* ���û�ȡ�����˳� */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"TRANS�߳��˳�");
	
	return 0;
}

/**
 *	_trans		-				����ת��,���ת������(����·�ر�)����رո��׽��ֶԣ����黹��Դ
 *
 *	@local			[in/out]	ת���߳��õ��ľֲ��������
 *	@pNode			[in]		ת���õ����׽��ֶ�
 *	@isSrv			[in]		ת���ķ���
 *
 *
 *	return
 *		==0						�ɹ�ת��
 *		!=0						������ error.h
 */
int MTrans::_trans(MLOCALTRANS &local, TRANS_SOCKET_PEER *pNode, bool isSrv)
{
	int							rc;

	if(isSrv)
		rc = _srvToCli(pNode);
	else
		rc = _cliToSrv(pNode);

	if(rc != 0)
	{
		Global_Log.writeLog(LOG_TYPE_INFO, 
			"TRANSת��ʱ����[%d(%d:%d)],���ر��׽��ֶ�[%d:%d],��ת���ֽ���[%u,%u],�����ֽ���[%u, %u]",
			rc, GET_SYS_ERR(rc), GET_USER_ERR(rc),
			pNode->m_cli.m_sock, pNode->m_srv.m_sock,
			pNode->m_cli.m_tranSize, pNode->m_srv.m_tranSize,
			pNode->m_cli.m_engine.leaveParased, pNode->m_srv.m_engine.leaveParased);

		assert(pNode->m_cli.m_engine.leaveParased == 0);
		assert(pNode->m_srv.m_engine.leaveParased == 0);

		MSocket::close(pNode->m_cli.m_sock);
		MSocket::close(pNode->m_srv.m_sock);

		local.del(pNode);/* ��Դ�ع� */		
	}

	return rc;
}

/**
 *	_srvToCli		-			SRV��CLI������ת��
 *
 *	@peer			[in/out]	�׽��ֶ���Ϣ
 *
 *
 *	return
 *		==0						�ɹ�ת��
 *		!=0						�ϼ��رո��׽��ֶ�
 */
int MTrans::_srvToCli(TRANS_SOCKET_PEER* peer)
{
	assert(peer != NULL && 
		peer->m_cli.m_sock != INVALID_SOCKET && 
		peer->m_srv.m_sock != INVALID_SOCKET);

	SOCKET_INFO				&sockInfo = peer->m_srv;	
	unsigned int			&leaveParased = sockInfo.m_engine.leaveParased;
	HTTP_PARASE_PARAM		&httpParam = sockInfo.m_engine.httpParase;
	LINE_PARASE_PARAM		&headParam = sockInfo.m_engine.lineParase;	
	int						onceRecved = 0;
	int						ret;
	
	onceRecved = recv(sockInfo.m_sock, 
		sockInfo.m_sockBuffer + leaveParased,
		sizeof(sockInfo.m_sockBuffer) - leaveParased,
		0);
	
	if(onceRecved <= 0)
		return BUILD_ERROR(_OSerrno(), ESCLOSE);

	ret = MHttpParase::parase(sockInfo.m_sockBuffer, 
		onceRecved+leaveParased, 
		httpParam, 
		headParam,
		peer->m_cli.m_sock,
		peer->m_srv.m_tranSize);
	assert(ret <= onceRecved+(int)leaveParased);
	if(ret < 0)
	{
		assert(0);
		return BUILD_ERROR(0, EPARASE);
	}
	
	leaveParased = onceRecved+leaveParased-ret;
	
	memmove(sockInfo.m_sockBuffer, sockInfo.m_sockBuffer+ret, leaveParased);
	
	return 0;
}
/**
 *	_cliToSrv		-			CLI��SRV������ת��
 *
 *	@peer			[in/out]	�׽��ֶ���Ϣ
 *
 *
 *	return
 *		==0						�ɹ�ת��
 *		!=0						�ϼ��رո��׽��ֶ�
 */
int MTrans::_cliToSrv(TRANS_SOCKET_PEER* peer)
{
	assert(peer != NULL);
	assert(peer->m_cli.m_sock != INVALID_SOCKET);
	assert(peer->m_srv.m_sock != INVALID_SOCKET);

	int							rc,_rc;
	char						*pSend = NULL;

	rc = recv(peer->m_cli.m_sock, 
		peer->m_cli.m_sockBuffer+MAX_HTTPHEAD_SIZE, 
		MAX_IOBUFFER_LEN, 0);
	if(rc <= 0)
		return BUILD_ERROR(_OSerrno(), ECCLOSE);

	_rc = rc;
	peer->m_cli.m_engine.leaveParased += _rc;	/*�����ֽ����ۼ�*/

	rc = MHttpParase::buildHttpData(peer->m_cli.m_sockBuffer+MAX_HTTPHEAD_SIZE,
		rc,
		MAX_HTTPHEAD_SIZE,
		&pSend,
		0,
		0); /* ���Ե�:�̶���0�Ƿ���ȱ�� */

	if(rc < 0)
		return rc;

	if(send(peer->m_srv.m_sock, pSend, rc, 0) != rc)
		return BUILD_ERROR(_OSerrno(), ESCLOSE);

	/* ͳ��ת������*/
	peer->m_cli.m_tranSize += _rc;
	/*�����ֽ����ۼ�*/
	peer->m_cli.m_engine.leaveParased -= _rc;

	return 0;
}



