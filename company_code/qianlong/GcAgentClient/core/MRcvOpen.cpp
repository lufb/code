#include <assert.h>
#include "MRcvOpen.h"
#include "MLocalLock.h"
#include "error.h"
#include "MList.h"
#include "MGlobal.h"
#include "MSocket.h"
#include "MBaseFunc.h"
#include "gcCliInterface.h"
#include "MBaseConfig.h"


MAcceptMgr::MAcceptMgr()
{
	INIT_LIST_HEAD(&m_stFree);
	INIT_LIST_HEAD(&m_stUsed);
	InitializeCriticalSection(&m_Lock);


	/** add node to m_stFree:need not to lock in constructor*/
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_stSockList[i].m_sock = INVALID_SOCKET;		/** init m_sock*/
		list_add_tail(&(m_stSockList[i].m_listNode), &m_stFree);
	}
}

MAcceptMgr::~MAcceptMgr()
{
	DeleteCriticalSection(&m_Lock);
}

/**
 *	insert		-				�����������һ��accept����׽���
 *
 *	@accSock:		[in]		accept����׽���
 *
 *
 *	return
 *		==0						��ӳɹ�
 *		!=0						���ʧ��
 */
int	MAcceptMgr::insert(SOCKET accSock)
{
	MLocalSection					locSec;

	locSec.Attch(&m_Lock);

	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ELINKOVERFLOW);

	/** fill sock*/
	SOCKET_LIST	*pNode = list_entry(m_stFree.next, SOCKET_LIST, m_listNode);
	pNode->m_sock = accSock;
	
	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);	
}

/**
 *	delOneFromUsed		-		��used������ȡһ���׽��֣����ȡ�����������黹free��
 *
 *	@sock:			[in/out]	ȡ�����׽���
 *
 *
 *	return
 *		==0						ȡ�ɹ�
 *		!=0						ȡʧ��
 */
int	MAcceptMgr::del(SOCKET &sock)
{
	MLocalSection					locSec;							
	
	sock = INVALID_SOCKET;

	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** �����*/
		return BUILD_ERROR(0, ELISTEMPTY);

	SOCKET_LIST	*pNode = list_entry(m_stUsed.next, SOCKET_LIST, m_listNode);
	sock = pNode->m_sock;

	pNode->m_sock = INVALID_SOCKET;
	
	return MList::_del2insert(&(pNode->m_listNode), &m_stFree);
}

/**
 *	destroy		-			�ͷ���Դ,���ü���(�Ѿ�û�����������)
 *
 *
 *
 *	return
 *								��					
 */
void MAcceptMgr::destroy()
{
	struct list_head				*pList, *pSafe;
	SOCKET_LIST						*pNode;
	int								total = 0;

	list_for_each_safe(pList, pSafe, &m_stUsed)
	{
		++total;
		pNode = list_entry(pList, SOCKET_LIST, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��accepted�����йر��׽���[%d]",
			pNode->m_sock);
		MSocket::close(pNode->m_sock);
		pNode->m_sock = INVALID_SOCKET;
		list_del(pList);
		list_add_tail(pList, &m_stFree);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��accepted�����й�ɾ��[%d]���׽���",
		total);
	}	
}

//////////////////////////////////////////////////////////////////////////



/**
 *	_MLOCALRCVOPEN_		-		���캯�����������߳��ж���ֲ�����ʱ�ĳ��绯
 */
_MLOCALRCVOPEN_::_MLOCALRCVOPEN_()
{
	INIT_LIST_HEAD(&m_stUsed);
	INIT_LIST_HEAD(&m_stFree);
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_sDataArr[i].m_sSock = INVALID_SOCKET;
		list_add_tail(&(m_sDataArr[i].m_lListNode), &m_stFree);
	}
}

/**
 *	~_MLOCALRCVOPEN_		-		�����������߳��˳��󣬸þֲ�����������������׽��ֵĹر�
 */
_MLOCALRCVOPEN_::~_MLOCALRCVOPEN_()
{
	struct list_head	*pTmp;
	MRCVOPEN_NODE		*pNode;
	int					total = 0;	/* �洢�������׽��ֶԣ�����鿴���� */
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, MRCVOPEN_NODE, m_lListNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"RCVOPEN�߳��˳��󣬽��ر��׽���[%d]",
			pNode->m_sSock);
		MSocket::close(pNode->m_sSock);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"RCVOPEN�߳��˳�ʱ,����[%d]���׽����ڵȴ��û�������֤��Ϣ",
		total);
	}	
}

/**
 *	insert		-				�������
 *
 *	@sock			[in]		��ӵ��׽���		
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	_MLOCALRCVOPEN_::insert(SOCKET sock)
{
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ELINKOVERFLOW);
	
	/** fill sock*/
	MRCVOPEN_NODE	*pNode = list_entry(m_stFree.next, MRCVOPEN_NODE, m_lListNode);
	pNode->m_sSock = sock;
	
	return MList::_del2insert(&(pNode->m_lListNode), &m_stUsed);
}

/**
 *	del		-					�黹��Դ������Ӧ�ĳ�ʼ��
 *
 *	@pNode			[in]		Ҫɾ���Ľڵ�
 *
 *	return
 *					��
 */
void _MLOCALRCVOPEN_::del(MRCVOPEN_NODE *pNode)
{
	pNode->m_sSock = INVALID_SOCKET;
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
int _MLOCALRCVOPEN_::isFull()
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
int	_MLOCALRCVOPEN_::isEmpty()
{
	return list_empty(&m_stUsed);
}



//////////////////////////////////////////////////////////////////////////

MRcvOpen::MRcvOpen()
{

}

MRcvOpen::~MRcvOpen()
{

}



int	MRcvOpen::init()
{
	int								rc;
	
	rc = m_clConnModule.init();
	if(rc != 0)
		return rc;
	
	m_hRcvOpenHandle =  _beginthreadex(NULL, 0, rcvOpenThread, this, 0, NULL);
	if(m_hRcvOpenHandle == -1)
		return BUILD_ERROR(_OSerrno(), ECRETHREAD);
	
	return 0;
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
void	MRcvOpen::waitExit(int *err, size_t size)
{
	int						rc = 0;

	if(m_hRcvOpenHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_hRcvOpenHandle, WAIT_THREAD_TIMEOUT);				
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�RCVOPEN�߳��˳���ʱ[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�RCVOPEN�߳��˳�����[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}

		m_hRcvOpenHandle = 0;
	}
	
	/* �ͷű�����Դ */
	destroy();
	/* ��䱾������� */
	err[0] = rc;
	assert(size > 1);

	m_clConnModule.waitExit(++err, --size);
}

/**
 *	destroy		-			�ͷű�����Դ
 *
 *
 *	return
 *							��
 */
void MRcvOpen::destroy()
{
	m_stAcceptedMgr.destroy();
}


/**
 *	insert		-				�����������һ��accept����׽���
 *
 *	@accSock:		[in]		accept����׽���
 *
 *
 *	return
 *		==0						��ӳɹ�
 *		!=0						���ʧ��
 */
int MRcvOpen::insert(SOCKET accSock)
{
	return m_stAcceptedMgr.insert(accSock);
}

/**
 *	del		-					��accept�Ķ����е�һ��socket������������
 *								�������������buffer�ټӵ�����������
 *
 *	@sock:		[in/out]		������socket
 *
 *
 *	return
 *		==0						��ȡ�ɹ�
 *		!=0						��ȡʧ��
 */
int	MRcvOpen::_del(SOCKET &sock)
{
	return m_stAcceptedMgr.del(sock);
}


/**
 *	_fillType		-			����open��������ͺʹ�������(ί��/��ί��)
 *
 *	@rcvPro			[in]		�յ�Э������(ͷ+��)
 *	@size			[in]		�յ���Э�����ݳ���
 *	@gcPro			[in/out]	��䱾��Э��ṹ��
 *
 *	Note:
 *					�������size��
 *								�����ϲ���þ���ֻ��������Э��������ĳ��Ȼ���Э��������ĳ���
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ�� ��Ӧerror.h������
 */
int MRcvOpen::_fillType(const char *rcvPro, const size_t size, GC_PRO &gcPro)
{
	assert(NULL != rcvPro);
	tagQLGCProxy_Apply		*pApply = (tagQLGCProxy_Apply*)rcvPro;
	unsigned char			applyType;
	int						rc = 0;			/* ����ֵ */

	applyType = pApply->ucApplyType;
	if(applyType != 1 && applyType != 2)/* ֻ������������(��ί����ί��)��� */
		return BUILD_ERROR(0, EUSERPROTOCAL);

	gcPro.m_ucApplyType = applyType;	/* �������(ί��/��ί��)*/

	if(size == sizeof(tagQLGCProxy_Certify) + sizeof(tagQLGCProxy_Apply)){/* ��Э�� */
		gcPro.m_eType = DIRCONN;	/* ��Э��ֻ����ֱ�� */
		return 0;
	} else if(size == sizeof(tagNewQLGCProxy_Certify) + sizeof(tagQLGCProxy_Apply) ){/* ��Э�� */
		tagNewQLGCProxy_Certify			*pPro = (tagNewQLGCProxy_Certify *)(rcvPro+sizeof(tagQLGCProxy_Apply));
		
		if(memcmp(pPro->strMagic, GCC_PROXY_HEAD, strlen(GCC_PROXY_HEAD)))
			return BUILD_ERROR(0, E_ERR_MAAGIC);

		switch (pPro->ucType)		/* ����Э����Ϣ������ӷ�ʽ */
		{
		case 0x0:
			gcPro.m_eType = DIRCONN;
			break;
		case 0x1:
			gcPro.m_eType = SOCK4;
			break;
		case 0x2:
			gcPro.m_eType = SOCK5;
			break;
		case 0x3:
			gcPro.m_eType = HTTP1_1;
			break;
		case 0x4:
			gcPro.m_eType = SOCK4A;
			break;
		default:
			assert(0);
			gcPro.m_eType = INVALID_REG;/* ��Ϊ�Ƿ����� */
			rc = BUILD_ERROR(0, E_PRO_TYPE);
		}
	}else
	{
		assert(0);
		rc = BUILD_ERROR(0, EPARAM);
	}

	return rc;
}

/**
 *	_fillPro		-			����open����䱾��Э������
 *
 *	@rcvPro			[in]		�յ�Э������
 *	@size			[in]		�յ���Э�����ݳ���
 *	@gcPro			[in/out]	��䱾��Э��ṹ��
 *
 *	Note:
 *					�������size��
 *								�����ϲ���þ���ֻ��������Э��������ĳ��Ȼ���Э��������ĳ���
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ�� ��Ӧerror.h������
 */
int	MRcvOpen::_fillPro(const char *rcvPro, const size_t size, GC_PRO &gcPro)
{
	/* �������ɫͨ����IP��˿� */
	Global_Option.getGcSIPPort(gcPro.m_cGcSIP, sizeof(gcPro.m_cGcSIP), gcPro.m_usGcSPort);

	if(size == sizeof(tagQLGCProxy_Certify))				/* ��Э����䷽ʽ */
	{
		tagQLGCProxy_Certify	*pOldPro = (tagQLGCProxy_Certify *)rcvPro;

		strncpy(gcPro.m_cDstIP, pOldPro->strIPAddress, sizeof(gcPro.m_cDstIP)-1);
		gcPro.m_usDstPort = pOldPro->usPort;
#ifdef _DEBUG
		/* ��ӡ�û��Ĳ�������־���Է������ */
		Global_Log.writeLog(LOG_TYPE_INFO,
			"Old Protocol,strIPAddress[%s],usPort[%d], CheckCode[%d]",
			pOldPro->strIPAddress, pOldPro->usPort, pOldPro->CheckCode);
#endif
		return 0;
	}else if(size == sizeof(tagNewQLGCProxy_Certify))	/* ��Э����䷽ʽ */
	{
		tagNewQLGCProxy_Certify *pNewPro = (tagNewQLGCProxy_Certify *)rcvPro;

		gcPro.m_bNeedAuth = pNewPro->bAuth;
		strncpy(gcPro.m_cUrName, pNewPro->strUserName, sizeof(gcPro.m_cUrName)-1);
		strncpy(gcPro.m_cPassWd, pNewPro->strUserPassword, sizeof(gcPro.m_cPassWd)-1);
		strncpy(gcPro.m_cAgIP, pNewPro->strAgentAddress, sizeof(gcPro.m_cAgIP)-1);
		strncpy(gcPro.m_cDstIP, pNewPro->strIPAddress, sizeof(gcPro.m_cDstIP)-1);
		gcPro.m_usAgPort = pNewPro->usAgentPort;
		gcPro.m_usDstPort = pNewPro->usPort;
#ifdef _DEBUG
		/* ��ӡ�û��Ĳ�������־���Է������ */
		Global_Log.writeLog(LOG_TYPE_INFO,
			"New Protocol,ucType[%d], bAuth[%d], strUserName[%s], strUserPassword[%s],"
			"strAgentAddress[%s], strIPAddress[%s], usAgentPort[%d], usPort[%d], CheckCode[%d]",
			pNewPro->ucType,
			pNewPro->bAuth, pNewPro->strUserName, pNewPro->strUserPassword,
			pNewPro->strAgentAddress, pNewPro->strIPAddress, pNewPro->usAgentPort,
			pNewPro->usPort, pNewPro->CheckCode);
#endif

		return 0;
	}else
	{
		assert(0);	
		return BUILD_ERROR(0, EPARAM);
	}
}


/**
 *	_doRcvOpen		-			���û���������Ϣ
 *
 *	@pNode:			[in]		�ڵ���Ϣ
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int MRcvOpen::_doRcvOpen(MRCVOPEN_NODE *pNode)
{
	assert(pNode != NULL && pNode->m_sSock != INVALID_SOCKET);
	char					rcvBuffer[1024];	/* ��Э�����ݻ����� 1024���㹻�� */
	GC_PRO					gcPro;				/* ��Ҫ���ı���Э����Ϣ */
	unsigned short			bodySize;
	int						rc;

	/* step1: ��Э��ͷ*/
	rc = MSocket::rcvDataNonBlock(pNode->m_sSock, rcvBuffer, 
		sizeof(tagQLGCProxy_Apply));
	if(rc != 0)
		return rc;

	/* stpe2:У��ͷ */
	if(memcmp(rcvBuffer, USER_DATA_HEAD, strlen(USER_DATA_HEAD)) != 0)
		return BUILD_ERROR(0, EUSERPROTOCAL);
	
	/* step3: �õ�������ĳ���*/
	bodySize = ((tagQLGCProxy_Apply*)rcvBuffer)->usSize;
	if(bodySize == 0)
		return BUILD_ERROR(0, EUSERPROTOCAL);

	/* step4: �����С */
	rc = MSocket::rcvDataNonBlock(pNode->m_sSock, 
		rcvBuffer+sizeof(tagQLGCProxy_Apply), bodySize);
	if(rc != 0)
		return rc;

	gcPro.init();

	/* ���Э�����ͺʹ�������(ί��/��ί��) */
	if((rc = _fillType(rcvBuffer, bodySize+sizeof(tagQLGCProxy_Apply), gcPro)) != 0)
		return rc;

	/* ��䱾��Э������ */
	if((rc = _fillPro(rcvBuffer+sizeof(tagQLGCProxy_Apply), bodySize, gcPro)) != 0)
		return rc;

	/* ���ͻ����׽��� */
	gcPro.m_sCliSock = pNode->m_sSock;

	/* ���²���� */
	return m_clConnModule.insert(gcPro);
}




/**
 *	rcvOpenThread		-		���û���������Ϣ����
 *
 *	@accSock:		[in]		MConnect����ָ��
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
unsigned int __stdcall MRcvOpen::rcvOpenThread(void * in)
{
 	MRcvOpen				*pObj = (MRcvOpen* )in;
	struct timeval			tv;						/** the timeout of select*/
	MLOCALRCVOPEN			local;					/* ���߳�ʹ�õľֲ�����*/
	fd_set					fdRead;					/* �ɶ��׽��ּ���*/
	SOCKET					sock;					/* ��ʱ�׽��ֻ���*/
	struct list_head		*pList, *pSafe;
	MRCVOPEN_NODE			*pNode;
	int						rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);
	
	do{
		/** ���û��������ͼ���accept���׽��ֽ���*/
		if(!local.isFull())
		{
			if(pObj->_del(sock) == 0)	/* ȡaccept���׽��ֳɹ� */
			{
				assert(sock != INVALID_SOCKET);
				local.insert(sock); /* ����϶���ӳɹ�������У�鷵��ֵ */
			}
		}

		/* ������������׽��� */
		if(!local.isEmpty())
		{
			FD_ZERO(&fdRead);
			/** �������׽��ּ���select*/
			list_for_each(pList, &local.m_stUsed)
			{
				pNode = list_entry(pList, MRCVOPEN_NODE, m_lListNode); assert(pNode != NULL);
				FD_SET(pNode->m_sSock, &fdRead);
			}
			
			rc = select(0, NULL, &fdRead, NULL, &tv);
			if(rc > 0)	/* ���׽��ֿɶ� */
			{
				list_for_each_safe(pList, pSafe, &local.m_stUsed)
				{
					pNode = list_entry(pList, MRCVOPEN_NODE, m_lListNode); assert(pNode != NULL);
					if(FD_ISSET(pNode->m_sSock, &fdRead))
					{
						rc = pObj->_doRcvOpen(pNode);
						if(rc != 0){
							Global_Log.writeLog(LOG_TYPE_ERROR,
								"�׽���[%d]RCVOPEN����[%d], �ر��׽���[%d]", 
								pNode->m_sSock, rc, pNode->m_sSock);
							MSocket::close(pNode->m_sSock);
						}else{
							Global_Log.writeLog(LOG_TYPE_INFO,
								"�׽���[%d]�����RCVOPEN",
								pNode->m_sSock);
						}
						/* ��Դ�ع� */
						local.del(pNode);
					}
				}
			}else if(rc < 0)
			{
				assert(0);
				Global_Log.writeLog(LOG_TYPE_ERROR, "RCVOPEN�߳�select����[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
			}
		}else{ Sleep(15);}
	}while(Global_IsRun);

	/*  ���û�ȡ���Ż��˳� */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"RCVOPEN�߳��˳�");

	return 0;
}