#include <assert.h>
#include "MConnect.h"
#include "error.h"
#include "MGlobal.h"
#include "MList.h"
#include "MSocket.h"


/**
 *	_MLOCALCONNECT_		-		构造函数，用以在线程中定义局部变量时的初如化
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
 *	~_MLOCALCONNECT_		-		析构函数，线程退出后，该局部变量的析构来完成套接字的关闭
 */
_MLOCALCONNECT_::~_MLOCALCONNECT_()
{
	struct list_head	*pTmp;
	MCONNECT_NODE		*pNode;
	int					total = 0;	/* 存储遗留的套接字对，方便查看问题 */
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, MCONNECT_NODE, m_lListNode);
		MSocket::close(pNode->m_sCliSock);
		MSocket::close(pNode->m_sSrvSock);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"CONN线程退出后，将关闭套接字对[%d:%d]",
			pNode->m_sCliSock, pNode->m_sSrvSock);
		MSocket::close(pNode->m_sCliSock);
		MSocket::close(pNode->m_sSrvSock);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"CONN线程退出时,还有[%d]套接字对在等待连接建立成功",
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
 *	insert		-				往数据区添加一个accept后的套接字
 *
 *	@accSock:		[in]		accept后的套接字
 *
 *
 *	return
 *		==0						添加成功
 *		!=0						添加失败
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
 *	delOneFromUsed		-		在used链表中取一个套接字，如果取出，缓冲区归还free中
 *
 *	@sock:			[in/out]	取出的套接字
 *
 *
 *	return
 *		==0						取成功
 *		!=0						取失败
 */
int	MAcceptMgr::delOneFromUsed(SOCKET &sock)
{
	MLocalSection					locSec;							
	
	sock = INVALID_SOCKET;

	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** 链表空*/
		return BUILD_ERROR(0, ELISTEMPTY);

	SOCKET_LIST	*pNode = list_entry(m_stUsed.next, SOCKET_LIST, m_listNode);
	sock = pNode->m_sock;

	pNode->m_sock = INVALID_SOCKET;
	
	return MList::_del2insert(&(pNode->m_listNode), &m_stFree);
}

/**
 *	destroy		-			释放资源,不用加锁(已经没人在用这个了)
 *
 *
 *
 *	return
 *								无					
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
			"从accepted队列中关闭套接字[%d]",
			pNode->m_sock);
		MSocket::close(pNode->m_sock);
		pNode->m_sock = INVALID_SOCKET;
		list_del(pList);
		list_add_tail(pList, &m_stFree);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"从accepted队列中共删除[%d]个套接字",
		total);
	}	
}





//below is for test

#ifdef _DEBUG
//////////////////////////////////////////////////////////////////////////

/**
 *	printList		-			打印一个链表中的数据
 *
 *	@head:		[in]			需要打印的链表头
 *
 *
 *	return
 *								无
 */
void MAcceptMgr::printList(struct list_head* head)
{
	if(head == &m_stUsed)
		printf("打印使用的链表:\n");
	else if(head == &m_stFree)
		printf("打印空闲链表:\n");
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
 *	destroy		-			释放本层资源
 *
 *
 *	return
 *							无
 */
void MConnect::destroy()
{
	m_stAcceptedMgr.destroy();
}

/**
 *	waitExit		-		释放本层资源及等待下层退出
 *	
 *	Note:
 *		每层退出码定义：
 *					== 0	成功
 *					!= 0	失败
 *
 *	@err					存储从该层到下面所有层退出的退出码
 *	@size					err的大小(int的个数)
 *
 *	return
 *							无
 */
void	MConnect::waitExit(int *err, size_t size)
{
	int						rc = 0;

	if(m_hConnHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_hConnHandle, WAIT_THREAD_TIMEOUT);				
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待CONNECT线程退出超时[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待CONNECT线程退出出错[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}

		m_hConnHandle = 0;
	}
	
	/* 释放本层资源 */
	destroy();
	/* 填充本层错误码 */
	err[0] = rc;
	assert(size > 1);

	m_clHandModule.waitExit(++err, --size);
}

/**
 *	insert		-				往数据区添加一个accept后的套接字
 *
 *	@accSock:		[in]		accept后的套接字
 *
 *
 *	return
 *		==0						添加成功
 *		!=0						添加失败
 */
int MConnect::insert(SOCKET accSock)
{
	return m_stAcceptedMgr.insert(accSock);
}

/**
 *	del		-					从accept的队列中弹一个socket出来进行连接
 *								如果弹出，将该buffer再加到空闲链表中
 *
 *	@sock:		[in/out]		弹出的socket
 *
 *
 *	return
 *		==0						获取成功
 *		!=0						获取失败
 */
int	MConnect::_del(SOCKET &sock)
{
	return m_stAcceptedMgr.delOneFromUsed(sock);
}

/**
 *	_insertToLocal		-		连接线中往线程中的局部连接队列里加元素
 *
 *	@local:			[in/out]	MConnect对象指针
 *	@cliSock		[in]		客户端的套接字		
 *	@srvSock		[in]		服务端的套接字
 *	@state			[in]		服务端套接字状态
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
 *	_sendHello		-			发送Hello包
 *
 *	@srvSock:			[in]	服务端套接字
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
 *	_buildHello		-			组装hello包
 *
 *	@buffer:			[in]	缓冲区
 *	@bufsize			[in]	缓冲区大小
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int MConnect::_buildHello(char *buffer, size_t bufsize)
{
	unsigned int					rc;
	unsigned long					randNum = Global_Rand.getARand();

	if(bufsize < sizeof(HTTP_HELLO_FMT_UP))
		return BUILD_ERROR(0, EBUFLESS);

	rc = sprintf(buffer, HTTP_HELLO_FMT_UP, 
		randNum, GCA_HELLO, 0, 0, 0);		/* 测试点: 都填0 */

	if(rc > bufsize){
		assert(0);
		return BUILD_ERROR(0, EBUFLESS);
	}

	return rc;
}

/**
 *	_doSucess		-			检索可读可写套接字，并执行相应操作
 *
 *	@local:			[in/out]	连接队列
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
		"CONN线程操作套接字对[%d,%d]失败[%d(%d:%d)],关闭该套接字对", 
		cliSock, srvSock, errCode, GET_SYS_ERR(errCode), GET_USER_ERR(errCode));
}

/**
 *	connThread		-			连接线程的执行函数，负责将两端的链路建立好
 *
 *	@accSock:		[in]		MConnect对象指针
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
				"用户还没有GcSIP与GcSPort,等待用户进行设置");
#endif
			Sleep(15);
		}
	}

	if(Global_IsRun)
		Global_Log.writeLog(LOG_TYPE_INFO,
			"用户已经设了GcSIP与GcSPort,开始进行连接GcS");
	
	do{
		/** 如果没有满，试图添加accept的套接字进来*/
		if(!list_empty(&localConnData.m_stFree))
		{
			/* 还有accept出来的套接字*/
			if(pConnObj->_del(cliSock) == 0)
			{
				assert(cliSock != INVALID_SOCKET);
				/* 每次都重新获取GcSIP与GcSPort */
				Global_Option.getGcSIPPort(pConnObj->m_uiGcSIP, pConnObj->m_usGcSPort);
				if(pConnObj->m_uiGcSIP == INADDR_NONE || pConnObj->m_uiGcSIP == 0)
					Global_Log.writeLog(LOG_TYPE_ERROR, "用户设置的GcSIP与GcSPort为空");
				/*	异步连接*/
				rc = MSocket::asynConnect(pConnObj->m_uiGcSIP, pConnObj->m_usGcSPort, &srvSock);
				if(rc < 0)
				{
					pConnObj->_doFailed(cliSock, srvSock, BUILD_ERROR(_OSerrno(), ECONNECT));
				}else if(rc == 0)/* 直接成功，理论上很少发生*/
				{
					pConnObj->_doSucess(cliSock, srvSock);
				}else	/* 需要select来检测成功*/
				{
					pConnObj->_insertToLocal(localConnData, cliSock, srvSock);
				}
			}
		}
		
		if(!list_empty(&localConnData.m_stUsed))
		{
			FD_ZERO(&fdWrite);
			/** 把所有套接字加入*/
			list_for_each(pList, &localConnData.m_stUsed)
			{
				pNode = list_entry(pList, MCONNECT_NODE, m_lListNode);
				FD_SET(pNode->m_sSrvSock, &fdWrite);
			}

			rc = select(0, NULL, &fdWrite, NULL, &tv);
			if(rc > 0)	/** 成功*/
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
				if(rc < 0)/* 发生错误 */
					Global_Log.writeLog(LOG_TYPE_ERROR, "CONN线程select出错[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
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
		}else{ Sleep(15);}	/* 防止CPU过高 */
	} while(Global_IsRun);
	
	/*  仅用户取消才会退出 */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"CONN线程退出");

	return 0;
}

/**
 *	_pull	 -	从所用节点里面弹出一个节点，并做相应初始化
 *
 *	@pNode:		需要弹出的节点
 *	@head：		节点弹出后要加到空闲链表的链表头
 *
 *	return
 *				无
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
 *	_getSockErr		-		根据SOCKET描述符,得到指定SOCKET的最后一次错误码
 *
 *	@s:		SOCKET描述符
 *
 *	return
 *		>	0	错误代码
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


