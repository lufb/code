#include <assert.h>
#include "MConnect.h"
#include "error.h"
#include "MGlobal.h"
#include "MList.h"
#include "MSocket.h"

MRcvedOpenMgr::MRcvedOpenMgr()
{
	INIT_LIST_HEAD(&m_stFree);
	INIT_LIST_HEAD(&m_stUsed);
	InitializeCriticalSection(&m_Lock);

	/** add node to m_stFree:need not to lock in constructor*/
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_stRcvOpenArr[i].m_stGcPro.init();		/* init node */
		list_add_tail(&(m_stRcvOpenArr[i].m_listNode), &m_stFree);
	}
}

MRcvedOpenMgr::~MRcvedOpenMgr()
{
	DeleteCriticalSection(&m_Lock);
}

/**
 *	insert		-				�����������һ�������open���Ľڵ�
 *
 *	@gcPro:			[in]		�յ���Э����
 *
 *
 *	return
 *		==0						��ӳɹ�
 *		!=0						���ʧ��
 */
int	MRcvedOpenMgr::insert(GC_PRO &gcPro)
{
	MLocalSection					locSec;

	locSec.Attch(&m_Lock);
	if(list_empty(&m_stFree)) /* list empty */
		return BUILD_ERROR(0, ELINKOVERFLOW);

	RCVED_OPEN_NODE *pNode = list_entry(m_stFree.next, RCVED_OPEN_NODE, m_listNode);
	assert(pNode != NULL);

	/* fill node */
	memcpy(&(pNode->m_stGcPro), &gcPro, sizeof(GC_PRO));

	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
}

/**
 *	_del		-				��mgr������ȡ����
 *
 *	@gcPro		[in/out]		ȡ��������
 *
 *
 *	return
 *		==0						ȡ�ɹ�
 *		!=0						ȡʧ��
 */
int	MRcvedOpenMgr::del(GC_PRO &gcPro)
{
	MLocalSection					locSec;							

	gcPro.init();
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** �����*/
		return BUILD_ERROR(0, ELISTEMPTY);
 
 	RCVED_OPEN_NODE	*pNode = list_entry(m_stUsed.next, RCVED_OPEN_NODE, m_listNode);
	assert(pNode != NULL);
 
	/*fill data*/
	memcpy(&gcPro, &(pNode->m_stGcPro), sizeof(pNode->m_stGcPro));
 	
	/* ��Դ�ع�ǰ���³�ʼ�� */
	pNode->m_stGcPro.init();
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
void MRcvedOpenMgr::destroy()
{
	struct list_head				*pList, *pSafe;
	SOCKET_LIST						*pNode;
	int								total = 0;

	list_for_each_safe(pList, pSafe, &m_stUsed)
	{
		++total;
		pNode = list_entry(pList, SOCKET_LIST, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��RCVDOPEN�����йر��׽���[%d]",
			pNode->m_sock);
		MSocket::close(pNode->m_sock);
		pNode->m_sock = INVALID_SOCKET;
		list_del(pList);
		list_add_tail(pList, &m_stFree);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"��RCVDOPEN�����й�ɾ��[%d]���׽���",
		total);
	}	
}


//////////////////////////////////////////////////////////////////////////


/**
 *	_MLOCALCONNECT_		-		���캯�����������߳��ж���ֲ�����ʱ�ĳ��绯
 */
_MLOCALCONNECT_::_MLOCALCONNECT_()
{
	INIT_LIST_HEAD(&m_stUsed);
	INIT_LIST_HEAD(&m_stFree);
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		memset(&(m_sDataArr[i].m_stGcPro), 0, sizeof(m_sDataArr[i].m_stGcPro));
		list_add_tail(&(m_sDataArr[i].m_listNode), &m_stFree);
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
		pNode = list_entry(pTmp, MCONNECT_NODE, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"CONN�߳��˳��󣬽��ر��׽��ֶ�[%d:%d]",
			pNode->m_stGcPro.m_sCliSock, pNode->m_stGcPro.m_sSrvSock);
		MSocket::close(pNode->m_stGcPro.m_sCliSock);
		MSocket::close(pNode->m_stGcPro.m_sSrvSock);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"CONN�߳��˳���,���ͷ�[%d]���׽���",
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
int	_MLOCALCONNECT_::insert(GC_PRO &gcPro)
{
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ELINKOVERFLOW);
	
	MCONNECT_NODE	*pNode = list_entry(m_stFree.next, MCONNECT_NODE, m_listNode);
	memcpy(&(pNode->m_stGcPro), &gcPro, sizeof(gcPro));
	
	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
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
void _MLOCALCONNECT_::del(MCONNECT_NODE *pNode)
{
	assert(pNode != NULL);

	pNode->m_stGcPro.init();
	list_del(&(pNode->m_listNode));
	list_add_tail(&(pNode->m_listNode), &m_stFree);
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
int _MLOCALCONNECT_::isFull()
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
int	_MLOCALCONNECT_::isEmpty()
{
	return list_empty(&m_stUsed);
}

//////////////////////////////////////////////////////////////////////////




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

	rc = m_clAgeModule.init();
	if(rc != 0)
		return rc;

	m_hConnHandle =  _beginthreadex(NULL, 0, connThread, this, 0, NULL);
	if(m_hConnHandle == -1)
		return BUILD_ERROR(_OSerrno(), ECRETHREAD);
	
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
	m_stRcvedOpenMgr.destroy();
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

	m_clAgeModule.waitExit(++err, --size);
}


/**
 *	_del		-				��mgr������ȡ����
 *
 *	@gcPro		[in/out]		ȡ��������
 *
 *
 *	return
 *		==0						ȡ�ɹ�
 *		!=0						ȡʧ��
 */
int	MConnect::_del(GC_PRO &gcPro)
{
	return m_stRcvedOpenMgr.del(gcPro);
}


/**
 *	insert		-				�����������һ��accept����׽���
 *
 *	@gcPro			[in]		�յ���open���е�Э����Ϣ
 *
 *
 *	return
 *		==0						��ӳɹ�
 *		!=0						���ʧ��
 */
int MConnect::insert(GC_PRO &gcPro)
{
	return m_stRcvedOpenMgr.insert(gcPro);
}

/**
 *	_doSucess		-			�����ɶ���д�׽��֣���ִ����Ӧ����
 *
 *	@local:			[in/out]	���Ӷ���
 *
 *
 *	return
 *								��
 */
void	MConnect::_doSucess(GC_PRO &gcPro)
{
	int							rc;

	assert(gcPro.m_sSrvSock != INVALID_SOCKET);

	/* ���÷�����׽�������*/
	MSocket::setSock(gcPro.m_sSrvSock, SOCK_RCV2SND_BUFF_SIZE);

	rc = m_clAgeModule.insert(gcPro);
	if(rc != 0)
		_doFailed(gcPro, rc);
}

void MConnect::_doFailed(GC_PRO &gcPro, int errCode)
{	
	Global_Log.writeLog(LOG_TYPE_ERROR, 
		"CONN�߳�����֤��ʽ[%d]�����׽��ֶ�[%d,%d]ʧ��[%d(%d:%d)],�رո��׽��ֶ�", 
		gcPro.m_eType, gcPro.m_sCliSock, gcPro.m_sSrvSock, errCode, GET_SYS_ERR(errCode), GET_USER_ERR(errCode));

	MSocket::close(gcPro.m_sCliSock);
	MSocket::close(gcPro.m_sSrvSock);
}

/**
 *	_getConnSrv		-			��Э����Ϣ�еõ���Ҫ���ӵķ�����IP��˿�
 *
 *	@gcPro:			[in]		Э������
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int MConnect::_getConnSrv(const GC_PRO &gcPro)
{
	assert(gcPro.m_eType != DIRCONN);

	char				ip[256] = {0};
	int					rc;

	Global_Log.writeLog(LOG_TYPE_INFO,
		"�׽��ֶ�[%d]Ҫ���ӵĴ��������Ϊ[%s:%d], ��������[%d]",
		gcPro.m_sCliSock, gcPro.m_cAgIP, gcPro.m_usAgPort, gcPro.m_eType);

	/* dnsת�� */
	rc = MSocket::dnsToIP((char *)&gcPro.m_cAgIP, sizeof(gcPro.m_cAgIP), ip, sizeof(ip)-1);
	if( rc != 0 ){
		Global_Log.writeLog(LOG_TYPE_WARN,
			"CONN�ж�[%s]����DNS����[%d]", 
			gcPro.m_cAgIP, rc);
		return rc;
	}
	
	/* ת��Ϊ�����ֽ��� */
	m_uiConnIP = inet_addr(ip);
	m_usConnPort = htons(gcPro.m_usAgPort);

	return 0;
}



/**
 *	_upConnSrv		-			�������ӵķ�����IP��˿�
 *
 *	@gcPro:			[in]		Э������
 *
 *
 *	return
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int MConnect::_upConnSrv( GC_PRO &gcPro)
{
	if(gcPro.m_eType == DIRCONN) /* ֱ��������ɫͨ����IP��˿� */
	{
		m_uiConnIP = inet_addr(gcPro.m_cGcSIP);
		m_usConnPort = htons(gcPro.m_usGcSPort);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"�׽��ֶ�[%d]Ҫ���ӵ���ɫͨ��������Ϊ[%s:%d]",
			gcPro.m_sCliSock, gcPro.m_cGcSIP, gcPro.m_usGcSPort);

		return 0;
	}else
	{
		return _getConnSrv(gcPro);
	}
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
	MConnect				*pObj = (MConnect*)in;
	MLOCALCONNECT			local;					/** local object*/
	GC_PRO					gcPro;
	fd_set					fdWrite;
	struct list_head		*pList, *pSafe;
	SOCKET					srvSock;
	struct timeval			tv;						/** the timeout of select*/
	MCONNECT_NODE			*pNode;
	int						rc;


	INIT_TIMEVAL(tv, SELECT_TIME_OUT);	
	do{
		/* ����û�� */
		if(!local.isFull())
		{
			if(pObj->_del(gcPro) == 0)/* ȡ������ */
			{
				assert(gcPro.m_sCliSock != INVALID_SOCKET && gcPro.m_sSrvSock == INVALID_SOCKET);
				/* ����Ҫ���ӵķ�����IP��˿� */
				rc = pObj->_upConnSrv(gcPro);
				if(rc != 0){
					Global_Log.writeLog(LOG_TYPE_WARN, 
						"�׽��ָ������ӵķ�����ʧ��,�رո��׽���", 
						gcPro.m_sCliSock, gcPro.m_sCliSock);
					MSocket::close(gcPro.m_sCliSock);
					continue;
				}

				rc = MSocket::asynConnect(pObj->m_uiConnIP, pObj->m_usConnPort, &srvSock);
				gcPro.m_sSrvSock = srvSock;	/* ��������׽��� */
				if(rc < 0)
				{
					pObj->_doFailed(gcPro,  BUILD_ERROR(_OSerrno(), ECONNECT));
				}else if(rc == 0)/* ֱ�ӳɹ��������Ϻ��ٷ���*/
				{
					pObj->_doSucess(gcPro);
				}else	/* ��Ҫselect�����ɹ�*/
				{
					local.insert(gcPro);
				}
			}
		}
		
		if(!local.isEmpty())
		{
			FD_ZERO(&fdWrite);
			/** �������׽��ּ���*/
			list_for_each(pList, &local.m_stUsed)
			{
				pNode = list_entry(pList, MCONNECT_NODE, m_listNode);
				FD_SET(pNode->m_stGcPro.m_sSrvSock, &fdWrite);
			}

			rc = select(0, NULL, &fdWrite, NULL, &tv);
			if(rc > 0)	/** �ɹ�*/
			{
				list_for_each_safe(pList, pSafe, &local.m_stUsed)
				{
					pNode = list_entry(pList, MCONNECT_NODE, m_listNode);
					if(FD_ISSET(pNode->m_stGcPro.m_sSrvSock, &fdWrite))
					{
						pObj->_doSucess(pNode->m_stGcPro);
						local.del(pNode);		
					}	
				}
			}else{
				if(rc < 0)/* �������� */
					Global_Log.writeLog(LOG_TYPE_ERROR, "CONN�߳�select����[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
				list_for_each_safe(pList, pSafe, &local.m_stUsed)
				{
					pNode = list_entry(pList, MCONNECT_NODE, m_listNode);
					if((rc = pObj->_getSockErr(pNode->m_stGcPro.m_sSrvSock)) != 0)
					{
						pObj->_doFailed(pNode->m_stGcPro, BUILD_ERROR(rc, ECONNECT));
						local.del( pNode );
					}				
				}
			}
		}else{Sleep(15);}
	}while(Global_IsRun);

	/*  ���û�ȡ���Ż��˳� */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"CONN�߳��˳�");

	return 0;
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


