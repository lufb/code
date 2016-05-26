#include <windows.h>
#include <assert.h>
#include "MHand.h"
#include "MGlobal.h"
#include "MList.h"
#include "MSocket.h"
#include "gcCliInterface.h"

/*
 *_MLOCALHAND_	-	���캯�������߳��ж���ֲ�����ʱ�ĳ�ʼ��
 */
_MLOCALHAND_::_MLOCALHAND_()
{
	INIT_LIST_HEAD(&m_stUsed);
	INIT_LIST_HEAD(&m_stFree);
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_sDataArr[i].m_peerState = RCVING_HELLO;
		m_sDataArr[i].m_cliSock = INVALID_SOCKET;
		m_sDataArr[i].m_srvSock = INVALID_SOCKET;
		list_add_tail(&(m_sDataArr[i].m_List), &m_stFree);
	}
}

/*
 *~_MLOCALHAND_	-	������������Hand�߳��˳�������׽��ֶԵĹر�
 */
_MLOCALHAND_::~_MLOCALHAND_()
{
	struct list_head		*pTmp;
	HAND_PEER				*pNode;
	int						total = 0;/* �洢�������׽��ֶԣ�����鿴���� */
	
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, HAND_PEER, m_List);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"HAND�߳��˳�,�׽��ֶ�״̬Ϊ[%d],���ر��׽��ֶ�[%d:%d],",
			pNode->m_peerState,
			pNode->m_cliSock, pNode->m_srvSock);
		MSocket::close(pNode->m_cliSock);
		MSocket::close(pNode->m_srvSock);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"HAND�߳��˳��󣬹��ͷ�[%d]���׽���",
			total);
	}
}

//////////////////////////////////////////////////////////////////////////

MConnedMgr::MConnedMgr()
{
	INIT_LIST_HEAD(&m_stFree);
	INIT_LIST_HEAD(&m_stUsed);
	InitializeCriticalSection(&m_Lock);
	
	/** init */
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_stConnArray[i].m_sCliSock = INVALID_SOCKET;
		m_stConnArray[i].m_sSrvSock = INVALID_SOCKET;
		list_add_tail(&(m_stConnArray[i].m_lListNode), &m_stFree);
	}

}

MConnedMgr::~MConnedMgr()
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
void MConnedMgr::destroy()
{
	struct list_head				*pList, *pSafe;
	CONN_NODE						*pNode;
	int								total = 0;
	
	list_for_each_safe(pList, pSafe, &m_stUsed)
	{
		++total;
		pNode = list_entry(pList, CONN_NODE, m_lListNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��connected�����йر��׽��ֶ�[%d]",
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
			"��connected�����й�ɾ��[%d]���׽��ֶ�",
		total);
	}	
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
int	MConnedMgr::insert(SOCKET cliSock, SOCKET srvSock)
{	
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);

	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, EHANDFULL);

	/* �����׽������� */
	_setSock(cliSock);
	_setSock(srvSock);
	
	/** fill sock*/
	CONN_NODE	*pNode = list_entry(m_stFree.next, CONN_NODE, m_lListNode);
	pNode->m_sCliSock = cliSock;
	pNode->m_sSrvSock = srvSock;

	Global_Log.writeLog(LOG_TYPE_INFO,
		"�׽��ֶ�[%d:%d]�������",
		cliSock, srvSock);	
	return MList::_del2insert(&(pNode->m_lListNode), &m_stUsed);
}

/**
 *	_setSock		-			�����׽��ֵ�����(����������������ջ�������С)
 *
 *	@sock			[in]		��Ҫ���õ��׽���
 *
 *	return
 *								��
 */
void MConnedMgr::_setSock(SOCKET sock)
{
	assert(sock != INVALID_SOCKET);

	if(MSocket::setnonblocking(sock) != 0)
		Global_Log.writeLog(LOG_TYPE_WARN,
			"�����׽���[%d]Ϊ����������", sock);

	if(MSocket::setSockBuf(sock, SOCK_RCV2SND_BUFF_SIZE) != 0)
		Global_Log.writeLog(LOG_TYPE_WARN,
			"�����׽���[%d]������(���������)����", sock);
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
int	MConnedMgr::del(SOCKET &cliSock, SOCKET &srvSock)
{
	MLocalSection					locSec;							
	
	cliSock = INVALID_SOCKET;
	srvSock = INVALID_SOCKET;
	
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** �����*/
		return BUILD_ERROR(0, ELISTEMPTY);
	
	CONN_NODE	*pNode = list_entry(m_stUsed.next, CONN_NODE, m_lListNode);
	cliSock = pNode->m_sCliSock;
	srvSock = pNode->m_sSrvSock;
	
	pNode->m_sCliSock = INVALID_SOCKET;
	pNode->m_sSrvSock = INVALID_SOCKET;
	
	return MList::_del2insert(&(pNode->m_lListNode), &m_stFree);
}


//////////////////////////////////////////////////////////////////////////

MHand::MHand()
{
	m_hHandHandle = 0;
}

MHand::~MHand()
{

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
void	MHand::waitExit(int *err, size_t size)
{
	int					rc = 0;
	
	if(m_hHandHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_hHandHandle, WAIT_THREAD_TIMEOUT);
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�HAND�߳��˳���ʱ[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�HAND�߳��˳�����[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}

		m_hHandHandle = 0;
	}
	
	/* �ͷű�����Դ */
	destroy();
	/* ��䱾������� */
	err[0] = rc;
	assert(size > 1);

	/* �ȴ��²㷵�� */
	m_clTranModule.waitExit(++err, --size);
}

int	MHand::init()
{
	int								rc;
	
	rc = m_clTranModule.init();
	if(rc != 0)
		return rc;
	
	m_hHandHandle =  _beginthreadex(NULL, 0, handThread, this, 0, NULL);
	
	return 0;
}

/**
 *	destroy		-				�ͷű�����Դ
 *
 *	return
 *								��
 */
void MHand::destroy()
{
	m_clConnetedMgr.destroy();
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
int	MHand::insert(SOCKET cliSock, SOCKET srvSock)
{
	return m_clConnetedMgr.insert(cliSock, srvSock);
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
int	MHand::del(SOCKET &cliSock, SOCKET &srvSock)
{
	return m_clConnetedMgr.del(cliSock, srvSock);
}

/**
 *	handThread		-			�����̵߳�ִ�к�������������
 *
 *	@accSock:		[in]		MHand����ָ��
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
unsigned int __stdcall MHand::handThread(void * in)
{
	assert(in != NULL);
	MHand					*pObj = (MHand*)in;
	MLOCALHAND				localHand;
	fd_set					fdRead;
	SOCKET					cliSock, srvSock;
	struct timeval			tv;	
	struct list_head		*pTmp, *pSafe;
	HAND_PEER				*pNode;
	int						rc,_rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);

	do{
		/* ����hand����û�� */
		if(!list_empty(&localHand.m_stFree))
		{
			/* ��ͼ��һ���Ѿ����Ӻõ��׽��ֶ� */
			if(pObj->m_clConnetedMgr.del(cliSock, srvSock) == 0)
			{
				assert(cliSock != INVALID_SOCKET);
				assert(srvSock != INVALID_SOCKET);
				/* �ӵ�����hand������ */
				pObj->_insertToLocal(localHand, cliSock, srvSock, RCVING_HELLO);
			}
		}

		if(!list_empty(&localHand.m_stUsed))
		{
			FD_ZERO(&fdRead);
			/* ��������������ӽ�select*/
			list_for_each(pTmp, &localHand.m_stUsed)
			{
				pNode = list_entry(pTmp, HAND_PEER, m_List);
				assert(pNode != NULL);
				if(pNode->m_peerState == RCVING_HELLO || 
					pNode->m_peerState == RCVING_UNIT_FROM_SRV){ /* ��SRV��HELLO�������ְ���Ӧ*/
					FD_SET(pNode->m_srvSock, &fdRead);
				}else if(pNode->m_peerState == RCVING_UNIT_FROM_CLI){	/* ��CLI�����ְ�*/
					FD_SET(pNode->m_cliSock, &fdRead);
				}else{ assert(0); }
			}
			
			rc = select(0, &fdRead, NULL, NULL, &tv);
			if(rc > 0)/* ���ɹ�*/
			{
				list_for_each_safe(pTmp, pSafe, &localHand.m_stUsed)
				{
					pNode = list_entry(pTmp, HAND_PEER, m_List);
					assert(pNode != NULL);
					if(FD_ISSET(pNode->m_cliSock, &fdRead)) {/* �ͻ���������*/
						_rc = pObj->_doRcv(pNode, false);
					}else if(FD_ISSET(pNode->m_srvSock, &fdRead)) {/* ������л�Ӧ*/
						_rc = pObj->_doRcv(pNode, true);
					}
					if(_rc != 0){ /* ���Ե� */
						pObj->_doFailed(localHand, pNode, _rc);
						pObj->_pull(pNode, &localHand.m_stFree);
					}else if(pNode->m_peerState == TRANSFORMING){
						pObj->_doSucess(localHand, pNode);
						pObj->_pull(pNode, &localHand.m_stFree);
					}				
				}
			}else
			{
				if(rc < 0){/* ��������*/
					Global_Log.writeLog(LOG_TYPE_ERROR, "HAND�߳�select����[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
					assert(0);
				}
				
			}
		}else{Sleep(15); }/* ��ֹCPU���� */
	}while(Global_IsRun);

	/* �����û�ȡ���Ż᷵�� */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"HAND�߳��˳�");

	return 0;
}

/**
 *	_pull		-			�黹��Դ������Ӧ�ĳ�ʼ��
 *
 *	@pNode			[in]		Ҫɾ���Ľڵ�
 *	@head			[in]		�ص��ĸ�������
 *
 *	return
 *					��
 */
void MHand::_pull(HAND_PEER *pNode, struct list_head *head)
{
	pNode->m_peerState = RCVING_HELLO;
	pNode->m_cliSock = INVALID_SOCKET;
	pNode->m_srvSock = INVALID_SOCKET;
	list_del(&(pNode->m_List));
	list_add_tail(&(pNode->m_List), head);
}

char* MHand::_StrStr(char* pBuf, int nSize, char* pSearch)
{
	int nPos = 0;
	int nStrLen = strlen(pSearch);
	while(nPos <= (nSize - nStrLen) ) 
	{
		if( memcmp(pBuf+nPos, pSearch, nStrLen) == 0 )
		{
			return pBuf+nPos;
		}
		nPos++;
	}
	return NULL;
}

/**
 *	_doFailed		-			����ʧ�ܣ�ɾ���׽��ֶԣ����黹��Դ
 *
 *	@localHand		[in/out]	�����������ݹ���
 *	@peer			[in]		��Ҫ�رյ��׽��ֶ�
 *	@errCode		[in]		�׽��ֶԳ���ԭ��
 *
 *	return
 *					��
 */
void MHand::_doFailed(MLOCALHAND &localHand, HAND_PEER *peer, int errCode)
{
	assert(peer != NULL);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"����Э�鷢������[%d(%d:%d)]״̬Ϊ[%d], �ر��׽��ֶ�[%d:%d]",
		errCode, GET_SYS_ERR(errCode),GET_USER_ERR(errCode),peer->m_peerState,
		peer->m_cliSock, peer->m_srvSock);

	MSocket::close(peer->m_cliSock);
	MSocket::close(peer->m_srvSock);
}

/**
 *	_doSucess		-			������ɣ����¼����Ԫ��
 *
 *	@localHand		[in/out]	�����������ݹ���
 *	@peeer			[in]		������ɵ��׽��ֶ�
 *
 *
 *	return
 *					��
 */
void MHand::_doSucess(MLOCALHAND &localHand, HAND_PEER *peer)
{
	int						rc;

	assert(peer != NULL && peer->m_peerState == TRANSFORMING);
	assert(peer->m_cliSock != INVALID_SOCKET && peer->m_srvSock != INVALID_SOCKET);
	
	rc = m_clTranModule.insert(peer->m_cliSock, peer->m_srvSock);
	if(rc != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
		"HAND�߳���TRANS�߳�����׽��ֶԷ�������[%d]���ر��׽��ֶ�[%d:%d]",
		rc, peer->m_cliSock, peer->m_srvSock);
		MSocket::close(peer->m_cliSock);
		MSocket::close(peer->m_srvSock);
	}	
}


/**
 *	_rcvHello		-			��hello��
 *
 *	@peer			[in/out]	�׽��ֶ�
 *
 *
 *	return
 *		==0						��ȫ�ɹ�
 *		!=0						ʧ��
 */
int	MHand::_rcvHello(HAND_PEER *peer)
{
	char						buffer[256];
	char						buffer2[256];
	int							rc,_rc;

	assert(peer->m_peerState == RCVING_HELLO);
	rc = _snprintf(buffer2, sizeof(buffer),
		HTTP_HELLO_FMT_DOWN, 0, 0, 0, 0);
	_rc = rc;

	rc = MSocket::rcvDataNonBlock(peer->m_srvSock, buffer, _rc); /** ���Ե�*/ 
	if(rc == 0)/* �ɹ�*/
		peer->m_peerState = RCVING_UNIT_FROM_CLI;

	assert(memcmp(buffer, buffer2, _rc) == 0);
	if(memcmp(buffer, buffer2, _rc) != 0)
		return BUILD_ERROR(0, EHELLO);

	return rc;
}




/**
 *	_insertToLocal		-		�ӵ�����hand������
 *
 *	@local			[in/out]	����hand�ṹ
 *	@cliSock:		[in]		�ͻ����׽���
 *	@srvSock:		[in]		������׽���
 *	@state			[in]		���׽��ֶԵ�״̬
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	MHand::_insertToLocal(MLOCALHAND &local,SOCKET cliSock, SOCKET srvSock, PEER_STATUS state)
{
	if(list_empty(&local.m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ESOCKOVERFLOW);
	
	/** fill sock*/
	HAND_PEER	*pNode = list_entry(local.m_stFree.next, HAND_PEER, m_List);
	pNode->m_peerState = state;
	pNode->m_cliSock = cliSock;
	pNode->m_srvSock = srvSock;
	
	return MList::_del2insert(&(pNode->m_List), &local.m_stUsed);
}



/**
 *	_rcvUnitFromSrv		-		��SRV�����ֻ�Ӧ��������Ӧ�ͻ���
 *
 *	@peer			[in/out]	�׽��ֶ�
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ�ܣ��ϼ���Ҫ�ر��׽��ֶ�
 */
int	MHand::_rcvUnitFromSrv(HAND_PEER *peer)
{
	char					rcvBuff[1024];
	char					commUnit[1024];
	char					tradeUnit[1024];
	int						rc,_rc;

	/* �õ�Ӧ���հ��Ĵ�С(��ͨunit��ί��unit��һ��) */
	rc = _snprintf(commUnit, sizeof(commUnit),
		HTTP_UNIT_FMT_DOWN, GCA_OPEN, 0, 0, 0);
	assert(rc <= sizeof(rcvBuff));

	rc = _snprintf(tradeUnit, sizeof(commUnit),
		HTTP_UNIT_FMT_DOWN, GCA_OPEN2, 0, 0, 0);
	assert(rc <= sizeof(rcvBuff));

	_rc = rc;	/* ����Ӧ�մ�С */

	rc = MSocket::rcvDataNonBlock(peer->m_srvSock, rcvBuff, _rc);
	if(rc != 0)
		return rc;

	/* У���յ��İ�����(����ί�����ί��) */
	if((memcmp(rcvBuff, commUnit, _rc) != 0)
		&&(memcmp(rcvBuff, tradeUnit, _rc) != 0))
	{
#define ERRCODE	"GCErCD:"
		char *pErrCode = _StrStr(rcvBuff, sizeof(rcvBuff)-1, ERRCODE);
		int	 errCode = atoi(pErrCode+sizeof(ERRCODE));
		Global_Log.writeLog(LOG_TYPE_WARN, 
			"����Э����������ش�����[%d]", errCode);
		return BUILD_ERROR(0, EHAND);
#undef ERRCODE
	}

	
	assert(sizeof(tagQLGCProxy_Apply)+sizeof(tagQLGCProxy_CertifyRet) <= sizeof(commUnit));
	tagQLGCProxy_Apply * pApplay = (tagQLGCProxy_Apply *)commUnit;
	tagQLGCProxy_CertifyRet * pCertify = (tagQLGCProxy_CertifyRet *)(commUnit + sizeof(tagQLGCProxy_Apply));
	
	memcpy(pApplay->szFlag, "QLGC", 4);
	pApplay->ucApplyType = 1;
	pApplay->usSize = sizeof(tagQLGCProxy_CertifyRet);
	pCertify->ucReply = 0;
	pCertify->CheckCode = '!';

	rc = MSocket::sendDataNonBlock(peer->m_cliSock, commUnit,
		sizeof(tagQLGCProxy_Apply)+sizeof(tagQLGCProxy_CertifyRet));
	if(rc == 0)
		peer->m_peerState =  TRANSFORMING;

	return rc;
}


/**
 *	_rcvUnitFromCli		-		��CLI�����ְ������ϱ�����
 ��
 *
 *	@peer			[in/out]	�׽��ֶ�
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ�ܣ��ϼ���Ҫ�ر��׽��ֶ�
 */
int	MHand::_rcvUnitFromCli(HAND_PEER *peer)
{
	char					rcvBuffer[1024];
	char					sndBuffer[1024];
	int						rc = 0;

	rc = MSocket::rcvDataNonBlock(peer->m_cliSock, rcvBuffer, 
		sizeof(tagQLGCProxy_Apply)+sizeof(tagQLGCProxy_Certify));
	if(rc != 0)
		return rc;

	tagQLGCProxy_Apply *pPro = (tagQLGCProxy_Apply *)rcvBuffer;
	tagQLGCProxy_Certify *pCer = (tagQLGCProxy_Certify *)(rcvBuffer+sizeof(tagQLGCProxy_Apply));
	if(pPro->ucApplyType == 1) {/* ��ͨ�����ְ� */
		rc = sprintf(sndBuffer, HTTP_UNIT_FMT_UP,
			pCer->strIPAddress, pCer->usPort,
			GCA_OPEN, 0, 0, 0);	/* ���Ե�: �����0 */
	}else if(pPro->ucApplyType == 2) {/* ί�е����ְ� */	
		rc = sprintf(sndBuffer, HTTP_UNIT_FMT_UP,
			pCer->strIPAddress, pCer->usPort,
			GCA_OPEN2, 0, 0, 0);/* ���Ե�: �����0 */
	}else{
		Global_Log.writeLog(LOG_TYPE_ERROR,
			"�ͻ��˷����������ְ�����[%d]����",
			pPro->ucApplyType);
		assert(0);
		BUILD_ERROR(0, EPARAM);
	}

	rc = MSocket::sendDataNonBlock(peer->m_srvSock, sndBuffer, rc);
	if(rc == 0)
		peer->m_peerState = RCVING_UNIT_FROM_SRV;

	return rc;
}


/**
 *	_doRcv			-			����ӦЭ�鲢������ӦЭ��
 *
 *	@peer			[in/out]	������׽��ֶ�
 *	@isSrv			[in]		���׽��ֶ��Ƿ���SRV
 *
 *	return
 *		==0						Э�鴦��ɹ�
 *		!=0						Э�鴦��ʧ�ܣ����׽��ֶ�Ӧ�ùر�
 */
int MHand::_doRcv(HAND_PEER *peer, bool isSrv)
{
	assert(peer != NULL);
	assert(peer->m_cliSock != INVALID_SOCKET);
	assert(peer->m_srvSock != INVALID_SOCKET);

	if(isSrv)/* ����� */
	{
		if(peer->m_peerState == RCVING_HELLO)
			return _rcvHello(peer);
		else if(peer->m_peerState == RCVING_UNIT_FROM_SRV)
			return _rcvUnitFromSrv(peer);
		else{
			Global_Log.writeLog(LOG_TYPE_ERROR,
				"״̬�д���[%d]�ˣ���ʱ��״̬Ϊ[%d]",
				isSrv, peer->m_peerState);
			assert(0);
			return BUILD_ERROR(0, ESTATUS);
		}
	}else/* �ͻ��� */
	{
		if(peer->m_peerState == RCVING_UNIT_FROM_CLI)
			return _rcvUnitFromCli(peer);
		else{
			Global_Log.writeLog(LOG_TYPE_ERROR,
				"״̬�д���[%d]�ˣ���ʱ��״̬Ϊ[%d]",
				isSrv, peer->m_peerState);
			assert(0);
			return BUILD_ERROR(0, ESTATUS);
		}
	}
}
