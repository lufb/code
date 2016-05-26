#include <assert.h>
#include "MConnect.h"
#include "error.h"
#include "MGlobal.h"
#include "MList.h"
#include "MSocket.h"


/**
 *	_MLOCALCONNECT_		-		���캯�����������߳��ж���ֲ�����ʱ�ĳ��绯
 */
_MLOCALCONNECT_::_MLOCALCONNECT_()
{
	INIT_LIST_HEAD(&m_stUsed);
	INIT_LIST_HEAD(&m_stFree);
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_sDataArr[i].m_sCliSock = INVALID_SOCKET;
		m_sDataArr[i].m_sSrvSock = INVALID_SOCKET;
		list_add_tail(&(m_sDataArr[i].m_lListNode), &m_stFree);
	}
}

/**
 *	~_MLOCALCONNECT_		-		�����������߳��˳��󣬸þֲ�����������������׽��ֵĹر�
 */
_MLOCALCONNECT_::~_MLOCALCONNECT_()
{
	struct list_head	*pTmp;
	MCONNECT_NODE		*pNode;
	int					total = 0;	/* �洢�������׽��ֶԣ�����鿴���� */
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, MCONNECT_NODE, m_lListNode);
		MSocket::close(pNode->m_sCliSock);
		MSocket::close(pNode->m_sSrvSock);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"CONN�߳��˳��󣬽��ر��׽��ֶ�[%d:%d]",
			pNode->m_sCliSock, pNode->m_sSrvSock);
		MSocket::close(pNode->m_sCliSock);
		MSocket::close(pNode->m_sSrvSock);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"CONN�߳��˳�ʱ,����[%d]�׽��ֶ��ڵȴ����ӽ����ɹ�",
		total);
	}	
}

//////////////////////////////////////////////////////////////////////////

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
		return BUILD_ERROR(0, ESOCKOVERFLOW);

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
int	MAcceptMgr::delOneFromUsed(SOCKET &sock)
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





//below is for test

#ifdef _DEBUG
//////////////////////////////////////////////////////////////////////////

/**
 *	printList		-			��ӡһ�������е�����
 *
 *	@head:		[in]			��Ҫ��ӡ������ͷ
 *
 *
 *	return
 *								��
 */
void MAcceptMgr::printList(struct list_head* head)
{
	if(head == &m_stUsed)
		printf("��ӡʹ�õ�����:\n");
	else if(head == &m_stFree)
		printf("��ӡ��������:\n");
	else
		assert(0);

	struct list_head			*pTmp;
	SOCKET_LIST					*sockList;
	int							i = 0;

	list_for_each(pTmp, head)
	{
		sockList = list_entry(pTmp, SOCKET_LIST, m_listNode);
		printf("index[%d]: socket[%d]\n", ++i, sockList->m_sock);
	}
		
}
void MAcceptMgr::printFree()
{
	printList(&m_stFree);	
}	
void MAcceptMgr::printUsed()
{
	printList(&m_stUsed);
}

#endif


//////////////////////////////////////////////////////////////////////////

MConnect::MConnect()
{
	m_hConnHandle = 0;
}

MConnect::~MConnect()
{

}

int MConnect::init()
{
	int								rc;

	rc = m_clHandModule.init();
	if(rc != 0)
		return rc;

	m_hConnHandle =  _beginthreadex(NULL, 0, connThread, this, 0, NULL);
	
	return 0;
}

/**
 *	destroy		-			�ͷű�����Դ
 *
 *
 *	return
 *							��
 */
void MConnect::destroy()
{
	m_stAcceptedMgr.destroy();
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
void	MConnect::waitExit(int *err, size_t size)
{
	int						rc = 0;

	if(m_hConnHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_hConnHandle, WAIT_THREAD_TIMEOUT);				
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�CONNECT�߳��˳���ʱ[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"�ȴ�CONNECT�߳��˳�����[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}

		m_hConnHandle = 0;
	}
	
	/* �ͷű�����Դ */
	destroy();
	/* ��䱾������� */
	err[0] = rc;
	assert(size > 1);

	m_clHandModule.waitExit(++err, --size);
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
int MConnect::insert(SOCKET accSock)
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
int	MConnect::_del(SOCKET &sock)
{
	return m_stAcceptedMgr.delOneFromUsed(sock);
}

/**
 *	_insertToLocal		-		�����������߳��еľֲ����Ӷ������Ԫ��
 *
 *	@local:			[in/out]	MConnect����ָ��
 *	@cliSock		[in]		�ͻ��˵��׽���		
 *	@srvSock		[in]		����˵��׽���
 *	@state			[in]		������׽���״̬
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	MConnect::_insertToLocal(MLOCALCONNECT &local, SOCKET cliSock, SOCKET srvSock)
{
	if(list_empty(&local.m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ESOCKOVERFLOW);
	
	/** fill sock*/
	MCONNECT_NODE	*pNode = list_entry(local.m_stFree.next, MCONNECT_NODE, m_lListNode);
	pNode->m_sCliSock = cliSock;
	pNode->m_sSrvSock = srvSock;
	
	return MList::_del2insert(&(pNode->m_lListNode), &local.m_stUsed);
}



/**
 *	_sendHello		-			����Hello��
 *
 *	@srvSock:			[in]	������׽���
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int MConnect::_sendHello(SOCKET srvSock)
{
	char					sendBuffer[512];
	int						rc;

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
int MConnect::_buildHello(char *buffer, size_t bufsize)
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
 *	_doSucess		-			�����ɶ���д�׽��֣���ִ����Ӧ����
 *
 *	@local:			[in/out]	���Ӷ���
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
void	MConnect::_doSucess(SOCKET cliSock, SOCKET srvSock)
{
	int							rc;

	rc = _sendHello(srvSock);
	if(rc != 0){
		_doFailed(cliSock, srvSock, rc);
		return;
	}
	
	rc = m_clHandModule.insert(cliSock, srvSock);
	if(rc != 0)
		_doFailed(cliSock, srvSock, rc);
}

void MConnect::_doFailed(SOCKET cliSock, SOCKET srvSock, int errCode)
{
	MSocket::close(cliSock);
	MSocket::close(srvSock);
	Global_Log.writeLog(LOG_TYPE_INFO, 
		"CONN�̲߳����׽��ֶ�[%d,%d]ʧ��[%d(%d:%d)],�رո��׽��ֶ�", 
		cliSock, srvSock, errCode, GET_SYS_ERR(errCode), GET_USER_ERR(errCode));
}

/**
 *	connThread		-			�����̵߳�ִ�к������������˵���·������
 *
 *	@accSock:		[in]		MConnect����ָ��
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
unsigned int __stdcall MConnect::connThread(void * in)
{
 	MConnect				*pConnObj = (MConnect*)in;
	MLOCALCONNECT			localConnData;			/** local object*/
	SOCKET					cliSock,srvSock;		/** to GcS socket tmp*/
	fd_set					fdWrite;
	struct timeval			tv;						/** the timeout of select*/
	struct list_head		*pList, *pSafe;
	MCONNECT_NODE			*pNode;
	int						rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);
	while(Global_IsRun)
	{
		if(Global_Option.isSeted())
			break;
		else{
#ifdef _DEBUG
			Global_Log.writeLog(LOG_TYPE_INFO,
				"�û���û��GcSIP��GcSPort,�ȴ��û���������");
#endif
			Sleep(15);
		}
	}

	if(Global_IsRun)
		Global_Log.writeLog(LOG_TYPE_INFO,
			"�û��Ѿ�����GcSIP��GcSPort,��ʼ��������GcS");
	
	do{
		/** ���û��������ͼ���accept���׽��ֽ���*/
		if(!list_empty(&localConnData.m_stFree))
		{
			/* ����accept�������׽���*/
			if(pConnObj->_del(cliSock) == 0)
			{
				assert(cliSock != INVALID_SOCKET);
				/* ÿ�ζ����»�ȡGcSIP��GcSPort */
				Global_Option.getGcSIPPort(pConnObj->m_uiGcSIP, pConnObj->m_usGcSPort);
				if(pConnObj->m_uiGcSIP == INADDR_NONE || pConnObj->m_uiGcSIP == 0)
					Global_Log.writeLog(LOG_TYPE_ERROR, "�û����õ�GcSIP��GcSPortΪ��");
				/*	�첽����*/
				rc = MSocket::asynConnect(pConnObj->m_uiGcSIP, pConnObj->m_usGcSPort, &srvSock);
				if(rc < 0)
				{
					pConnObj->_doFailed(cliSock, srvSock, BUILD_ERROR(_OSerrno(), ECONNECT));
				}else if(rc == 0)/* ֱ�ӳɹ��������Ϻ��ٷ���*/
				{
					pConnObj->_doSucess(cliSock, srvSock);
				}else	/* ��Ҫselect�����ɹ�*/
				{
					pConnObj->_insertToLocal(localConnData, cliSock, srvSock);
				}
			}
		}
		
		if(!list_empty(&localConnData.m_stUsed))
		{
			FD_ZERO(&fdWrite);
			/** �������׽��ּ���*/
			list_for_each(pList, &localConnData.m_stUsed)
			{
				pNode = list_entry(pList, MCONNECT_NODE, m_lListNode);
				FD_SET(pNode->m_sSrvSock, &fdWrite);
			}

			rc = select(0, NULL, &fdWrite, NULL, &tv);
			if(rc > 0)	/** �ɹ�*/
			{
				list_for_each_safe(pList, pSafe, &localConnData.m_stUsed)
				{
					pNode = list_entry(pList, MCONNECT_NODE, m_lListNode);
					if(FD_ISSET(pNode->m_sSrvSock, &fdWrite))
					{
						pConnObj->_doSucess(pNode->m_sCliSock, pNode->m_sSrvSock);
						pConnObj->_pull(pNode, &localConnData.m_stFree);		
					}	
				}
			}else{
				if(rc < 0)/* �������� */
					Global_Log.writeLog(LOG_TYPE_ERROR, "CONN�߳�select����[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
				list_for_each_safe(pList, pSafe, &localConnData.m_stUsed)
				{
					pNode = list_entry(pList, MCONNECT_NODE, m_lListNode);
					if((rc = pConnObj->_getSockErr(pNode->m_sSrvSock)) != 0)
					{
						pConnObj->_doFailed(pNode->m_sCliSock, pNode->m_sSrvSock, BUILD_ERROR(rc, ECONNECT));
						pConnObj->_pull(pNode, &localConnData.m_stFree);
					}				
				}
			}		
		}else{ Sleep(15);}	/* ��ֹCPU���� */
	} while(Global_IsRun);
	
	/*  ���û�ȡ���Ż��˳� */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"CONN�߳��˳�");

	return 0;
}

/**
 *	_pull	 -	�����ýڵ����浯��һ���ڵ㣬������Ӧ��ʼ��
 *
 *	@pNode:		��Ҫ�����Ľڵ�
 *	@head��		�ڵ㵯����Ҫ�ӵ��������������ͷ
 *
 *	return
 *				��
 *
 */
void MConnect::_pull(MCONNECT_NODE *pNode, struct list_head *head)
{
	assert(pNode != NULL && head != NULL);

	pNode->m_sCliSock = INVALID_SOCKET;
	pNode->m_sSrvSock = INVALID_SOCKET;
	list_del(&(pNode->m_lListNode));
	list_add_tail(&(pNode->m_lListNode), head);
}


/**
 *	_getSockErr		-		����SOCKET������,�õ�ָ��SOCKET�����һ�δ�����
 *
 *	@s:		SOCKET������
 *
 *	return
 *		>	0	�������
 *		<	0	remark
 *
 *	remark:
 *		a specific error code can be retrieved
 *	by calling GetLastError
 */
int MConnect::_getSockErr(SOCKET s)
{
	int errorcode = -2;
	int len = sizeof(int);

	if(!getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&errorcode, (int *)&len))
		return errorcode;
	else
		return -1;
}


