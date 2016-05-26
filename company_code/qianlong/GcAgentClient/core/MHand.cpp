#include <windows.h>
#include <assert.h>
#include "MHand.h"
#include "MGlobal.h"
#include "MList.h"
#include "MSocket.h"
#include "gcCliInterface.h"

void _CONN_::init()
{
	m_sCliSock = INVALID_SOCKET;
	m_sSrvSock = INVALID_SOCKET;
	m_cApplyType = 0;					/* 0�ǷǷ��ģ�1 ��ͨ 2 ί�� */
	memset(m_cDstIP, 0, sizeof(m_cDstIP));
	m_usDstPort = 0;
}
/////////////////////////////
/*
 *_MLOCALHAND_	-	���캯�������߳��ж���ֲ�����ʱ�ĳ�ʼ��
 */
_MLOCALHAND_::_MLOCALHAND_()
{
	INIT_LIST_HEAD(&m_stUsed);
	INIT_LIST_HEAD(&m_stFree);
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_sDataArr[i].m_peerState = HAND_CONN_ED;
		m_sDataArr[i].m_stAgentedNode.init();
		list_add_tail(&(m_sDataArr[i].m_listNode), &m_stFree);
	}
}

/*
 *~_MLOCALHAND_	-	������������Hand�߳��˳�������׽��ֶԵĹر�
 */
_MLOCALHAND_::~_MLOCALHAND_()
{
	struct list_head		*pTmp;
	HAND_LIST_NODE			*pNode;
	int						total = 0;/* �洢�������׽��ֶԣ�����鿴���� */
	
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, HAND_LIST_NODE, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"HAND�߳��˳�,�׽��ֶ�״̬Ϊ[%d],���ر��׽��ֶ�[%d:%d],",
			pNode->m_peerState,
			pNode->m_stAgentedNode.m_sCliSock, pNode->m_stAgentedNode.m_sSrvSock);
		MSocket::close(pNode->m_stAgentedNode.m_sCliSock);
		MSocket::close(pNode->m_stAgentedNode.m_sSrvSock);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"HAND�߳��˳��󣬹��ͷ�[%d]���׽���",
			total);
	}
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
int _MLOCALHAND_::isFull()
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
int	_MLOCALHAND_::isEmpty()
{
	return list_empty(&m_stUsed);
}

/**
 *	insert		-				�������
 *
 *	@agentNode	[in]			��ӵĽڵ�
 *	@state		[in]			�ڵ�״̬
 *
 *	Note:						ֻ�ڸ�ģ����߳���ʹ�ã����ü���
 *
 *	return
 *		!=0						��ӳɹ�
 *		==0						ʧ��
 */
int	_MLOCALHAND_::insert(AGENT_NODE &agentNode, PEER_STATUS state)
{
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ELINKOVERFLOW);
	
	/** fill sock*/
	HAND_LIST_NODE	*pNode = list_entry(m_stFree.next, HAND_LIST_NODE, m_listNode);
	pNode->m_peerState = state;
	memcpy(&(pNode->m_stAgentedNode), &agentNode, sizeof(AGENT_NODE));
	
	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
}

/**
 *	del		-				�黹��Դ������Ӧ�ĳ�ʼ��
 *
 *	@pNode			[in]		Ҫɾ���Ľڵ�
 *
 *	return
 *					��
 */
void _MLOCALHAND_::del(HAND_LIST_NODE *pNode)
{
	pNode->m_peerState = HAND_CONN_ED;
	pNode->m_stAgentedNode.init();
	list_del(&(pNode->m_listNode));
	list_add_tail(&(pNode->m_listNode), &m_stFree);
}

//////////////////////////////////////////////////////////////////////////

MAgentedMgr::MAgentedMgr()
{
	INIT_LIST_HEAD(&m_stFree);
	INIT_LIST_HEAD(&m_stUsed);
	InitializeCriticalSection(&m_Lock);
	
	/** init */
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_stAgentedArr[i].m_stAgeNode.init();
		list_add_tail(&(m_stAgentedArr[i].m_listNode), &m_stFree);
	}

}

MAgentedMgr::~MAgentedMgr()
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
void MAgentedMgr::destroy()
{
	struct list_head				*pList, *pSafe;
	AGENT_LIST_NODE					*pNode;
	int								total = 0;
	
	list_for_each_safe(pList, pSafe, &m_stUsed)
	{
		++total;
		pNode = list_entry(pList, AGENT_LIST_NODE, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��AGENTED�����йر��׽��ֶ�[%d:%d]",
			pNode->m_stAgeNode.m_sCliSock, pNode->m_stAgeNode.m_sSrvSock);
		MSocket::close(pNode->m_stAgeNode.m_sCliSock);
		MSocket::close(pNode->m_stAgeNode.m_sSrvSock);

		pNode->m_stAgeNode.init();

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
 *	@cliSock	[in]			�ͻ����׽���
 *	@srvSock	[in]			������׽���
 *	@dstIP		[in]			Ŀ�������IP
 *	@dstPort	[in]			Ŀ��������˿�
 *	&applyType	[in]			����(ί�����ί��)
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MAgentedMgr::insert(const SOCKET cliSock, const SOCKET srvSock, const char *dstIP, const unsigned short dstPort, const unsigned char applyType)
{	
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);

	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, EHANDFULL);
	
	/** fill sock*/
	AGENT_LIST_NODE	*pNode = list_entry(m_stFree.next, AGENT_LIST_NODE, m_listNode);
	pNode->m_stAgeNode.m_sCliSock = cliSock;
	pNode->m_stAgeNode.m_sSrvSock = srvSock;
	strncpy(pNode->m_stAgeNode.m_cDstIP, dstIP, sizeof(pNode->m_stAgeNode.m_cDstIP) - 1);
	pNode->m_stAgeNode.m_usDstPort = dstPort;
	pNode->m_stAgeNode.m_cApplyType = applyType;

	Global_Log.writeLog(LOG_TYPE_INFO,
		"�׽��ֶ�[%d:%d]�������(����AGENT����)",
		cliSock, srvSock);	
	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
}


/**
 *	del		-					ȡԪ��
 *
 *	@ageNode					ȡ����Ԫ��
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MAgentedMgr::del(AGENT_NODE	&ageNode)
{
	MLocalSection					locSec;							
	
	ageNode.init();
	
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** �����*/
		return BUILD_ERROR(0, ELISTEMPTY);
	
	AGENT_LIST_NODE	*pNode = list_entry(m_stUsed.next, AGENT_LIST_NODE, m_listNode);
	memcpy(&ageNode, &(pNode->m_stAgeNode), sizeof(ageNode));

	/* ��Դ�ع�ǰ���³�ʼ�� */
	pNode->m_stAgeNode.init();
	return MList::_del2insert(&(pNode->m_listNode), &m_stFree);
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
	if(m_hHandHandle == -1)
		return BUILD_ERROR(_OSerrno(), ECRETHREAD);
	
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
	m_clAgentedMgr.destroy();
}

/**
 *	insert		-			���Ԫ��
 *
 *	@cliSock	[in]			�ͻ����׽���
 *	@srvSock	[in]			������׽���
 *	@dstIP		[in]			Ŀ�������IP
 *	@dstPort	[in]			Ŀ��������˿�
 *	&applyType	[in]			����(ί�����ί��)
 *
 *	return
 *		== 0					�ɹ�
 *		!= 0					ʧ��
 */
int	MHand::insert(const SOCKET cliSock, const SOCKET srvSock, const char *dstIP, 
				  const unsigned short dstPort, const unsigned char applyType)
{
	return m_clAgentedMgr.insert(cliSock, srvSock, dstIP, dstPort, applyType);
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
	MHand					*pObj = (MHand*)in;
	MLOCALHAND				local;
	fd_set					fdRead;
	AGENT_NODE				agentedNode;
	struct timeval			tv;	
	struct list_head		*pList, *pSafe;
	HAND_LIST_NODE			*pNode;
	int						rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);

	do{
		if(!local.isFull())/* ����û�� */
		{
			if(pObj->m_clAgentedMgr.del(agentedNode) == 0)
			{
				if((rc = pObj->_sndHello(agentedNode)) == 0){	/* �ɹ�����HELLO�� */
					local.insert(agentedNode, HAND_SEND_HELLO_ED); /* ��ӵ����أ��϶��ǳɹ��ģ����ü��鷵��ֵ */
				}else{
					pObj->_doFailed(agentedNode, rc, HAND_CONN_ED);
				}
			}
		}

		if(!local.isEmpty())	/* ���������� */
		{
			FD_ZERO(&fdRead);
			list_for_each(pList, &local.m_stUsed)/* �����������select����*/
			{
				pNode = list_entry(pList, HAND_LIST_NODE, m_listNode);	assert(pNode != NULL);
				/* ����״̬���select */
				if(pNode->m_peerState == HAND_SEND_HELLO_ED || 
					pNode->m_peerState == HAND_SEND_OPEN_ED )
				{
					FD_SET(pNode->m_stAgentedNode.m_sSrvSock, &fdRead);
				}else{assert(0);}
			}

			rc = select(0, &fdRead, NULL, NULL, &tv);
			if(rc > 0)	/* ��⵽�׽��ֿɶ� */
			{
				list_for_each_safe(pList, pSafe, &local.m_stUsed)
				{
					pNode = list_entry(pList, HAND_LIST_NODE, m_listNode);	assert(pNode != NULL);
					if(FD_ISSET(pNode->m_stAgentedNode.m_sSrvSock, &fdRead))
					{
						rc = pObj->_doRcv(pNode);
						if(rc != 0)
						{
							pObj->_doFailed(pNode->m_stAgentedNode, rc, pNode->m_peerState);
							local.del(pNode);	/* ��Դ�ع� */
						}else if(pNode->m_peerState == HAND_RECV_OPEN_ED) /*����_doRcv�ɹ��Ż�ȥ�ж��Ƿ��������*/
						{
							pObj->_doSucess(pNode);					/* ���¼���� */
							local.del(pNode);	/* ��Դ�ع� */
						}
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
 *	_doFailed		-			ʧ��ʱ�Ĵ�ӡ��־���׽��ֹر�
 *
 *	@ageNode		[in]		����ڵ�
 *	@errCode		[in]		����ԭ��
 *	@state			[in]		����ʱ״̬
 *
 *	return
 *					��
 */
void MHand::_doFailed(AGENT_NODE &ageNode, int errCode, PEER_STATUS state)
{
	Global_Log.writeLog(LOG_TYPE_ERROR,
		"����Э�鷢������[%d(%d:%d)]״̬Ϊ[%d], �ر��׽��ֶ�[%d:%d]",
		errCode, GET_SYS_ERR(errCode),GET_USER_ERR(errCode),
		state, ageNode.m_sCliSock, ageNode.m_sSrvSock);

	MSocket::close(ageNode.m_sCliSock);
	MSocket::close(ageNode.m_sSrvSock);
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
void MHand::_doSucess(HAND_LIST_NODE *peer)
{
	int						rc;

	assert(peer != NULL && peer->m_peerState == HAND_RECV_OPEN_ED);
	assert(peer->m_stAgentedNode.m_sCliSock != INVALID_SOCKET && peer->m_stAgentedNode.m_sSrvSock != INVALID_SOCKET);
	
	rc = m_clTranModule.insert(peer->m_stAgentedNode.m_sCliSock, peer->m_stAgentedNode.m_sSrvSock);
	if(rc != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
		"HAND�߳���TRANS�߳�����׽��ֶԷ�������[%d]���ر��׽��ֶ�[%d:%d]",
		rc, peer->m_stAgentedNode.m_sCliSock, peer->m_stAgentedNode.m_sSrvSock);
		MSocket::close(peer->m_stAgentedNode.m_sCliSock);
		MSocket::close(peer->m_stAgentedNode.m_sSrvSock);
	}	
}


/**
 *	_rcvHello		-			��hello������open��
 *
 *	@peer			[in/out]	����Ľڵ�
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	MHand::_rcvHello(HAND_LIST_NODE *peer)
{
	char						buffer[256];
	char						buffer2[256];
	int							rc,_rc;

	assert(peer->m_peerState == HAND_SEND_HELLO_ED);
	rc = _snprintf(buffer2, sizeof(buffer),
		HTTP_HELLO_FMT_DOWN, 0, 0, 0, 0);
	_rc = rc;

	rc = MSocket::rcvDataNonBlock(peer->m_stAgentedNode.m_sSrvSock, buffer, _rc); /** ���Ե�*/ 
	if(rc != 0)
		return rc;

	peer->m_peerState = HAND_RECV_HELLO_ED;

	assert(memcmp(buffer, buffer2, _rc) == 0);
	if(memcmp(buffer, buffer2, _rc) != 0){
		assert(0);
		return BUILD_ERROR(0, EHELLO);
	}

	return _sndOpen(peer);
}

/**
 *	_sndOpen		-			��open��
 *
 *	@peer			[in/out]	����Ľڵ�
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int MHand::_sndOpen(HAND_LIST_NODE *peer)
{
	assert(peer != NULL && peer->m_peerState == HAND_RECV_HELLO_ED);
	assert(peer->m_stAgentedNode.m_cDstIP[0] != 0 && 
		peer->m_stAgentedNode.m_usDstPort != 0);

	char					sndBuffer[1024];
	int						rc = 0;

	if(peer->m_stAgentedNode.m_cApplyType == 1) {/* ��ͨ���ְ� */
		rc = sprintf(sndBuffer, HTTP_UNIT_FMT_UP,
			peer->m_stAgentedNode.m_cDstIP, 
			peer->m_stAgentedNode.m_usDstPort,
			GCA_OPEN, 0, 0, 0);	/* ���Ե�: �����0 */
	}else if(peer->m_stAgentedNode.m_cApplyType == 2){	/* ί�����ְ� */	
		rc = sprintf(sndBuffer, HTTP_UNIT_FMT_UP,
			peer->m_stAgentedNode.m_cDstIP, 
			peer->m_stAgentedNode.m_usDstPort,
			GCA_OPEN2, 0, 0, 0);/* ���Ե�: �����0 */
	}else{
		assert(0);
		return BUILD_ERROR(0, EPARAM);
	}

	rc = MSocket::sendDataNonBlock(peer->m_stAgentedNode.m_sSrvSock, sndBuffer, rc);
	if(rc == 0)
		peer->m_peerState = HAND_SEND_OPEN_ED;

	return rc;
}



/**
 *	_sndHello		-			����Hello��
 *
 *	@agentedNode:		[in]	����ɴ���Ľڵ�
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int MHand::_sndHello(AGENT_NODE agentedNode)
{
	char					sendBuffer[512];
	int						rc;
	SOCKET					srvSock = agentedNode.m_sSrvSock;

	assert(srvSock != INVALID_SOCKET);

	if(srvSock == INVALID_SOCKET)
		return BUILD_ERROR(0, EABORT);

	rc = _buildHello(sendBuffer, sizeof(sendBuffer));

	rc = MSocket::sendDataNonBlock( srvSock, sendBuffer, rc );
	if(rc != 0)
		return rc;

	return 0;
}

/**
 *	_buildHello		-			��װhello��
 *
 *	@buffer:			[in]	������
 *	@bufsize			[in]	��������С
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int MHand::_buildHello(char *buffer, size_t bufsize)
{
	unsigned int					rc;
	unsigned long					randNum = Global_Rand.getARand();

	if(bufsize < sizeof(HTTP_HELLO_FMT_UP))
		return BUILD_ERROR(0, EBUFLESS);

	rc = sprintf(buffer, HTTP_HELLO_FMT_UP, 
		randNum, GCA_HELLO, 0, 0, 0);		/* ���Ե�: ����0 */

	if(rc > bufsize){
		assert(0);
		return BUILD_ERROR(0, EBUFLESS);
	}

	return rc;
}





/**
 *	_rcvOpen		-			��open��������Ӧ�ͻ���
 *
 *	@peer			[in/out]	�հ���Ӧ�Ľڵ�
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ�ܣ��ϼ���Ҫ�ر��׽��ֶ�
 */
int	MHand::_rcvOpen(HAND_LIST_NODE *peer)
{
	assert(peer != NULL && peer->m_peerState == HAND_SEND_OPEN_ED);
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
	assert(rc < sizeof(rcvBuff));

	_rc = rc;	/* ����Ӧ�մ�С */

	rc = MSocket::rcvDataNonBlock(peer->m_stAgentedNode.m_sSrvSock, rcvBuff, _rc);
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

	rc = MSocket::sendDataNonBlock(peer->m_stAgentedNode.m_sCliSock, commUnit,
		sizeof(tagQLGCProxy_Apply)+sizeof(tagQLGCProxy_CertifyRet));
	if(rc == 0)
		peer->m_peerState =  HAND_RECV_OPEN_ED;

	return rc;
}

/**
 *	_doRcv			-			����ӦЭ�鲢������ӦЭ��
 *
 *	@peer			[in/out]	�����ݵĽڵ�
 *	return
 *		==0						Э�鴦��ɹ�
 *		!=0						Э�鴦��ʧ�ܣ����׽��ֶ�Ӧ�ùر�
 */
int MHand::_doRcv(HAND_LIST_NODE *peer)
{
	assert(peer != NULL && 
		peer->m_stAgentedNode.m_sCliSock != INVALID_SOCKET && 
		peer->m_stAgentedNode.m_sSrvSock != INVALID_SOCKET );

	if(peer->m_peerState == HAND_SEND_HELLO_ED){
		return _rcvHello(peer);
	}else if(peer->m_peerState == HAND_SEND_OPEN_ED){
		return _rcvOpen(peer);
	}else{
		assert(0);
		return BUILD_ERROR(0, EABORT);
	}

	return 0;
}
