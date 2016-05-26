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
 *	insert		-				往数据区添加一个完成收open包的节点
 *
 *	@gcPro:			[in]		收到的协议体
 *
 *
 *	return
 *		==0						添加成功
 *		!=0						添加失败
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
 *	_del		-				在mgr数据区取数据
 *
 *	@gcPro		[in/out]		取出的数据
 *
 *
 *	return
 *		==0						取成功
 *		!=0						取失败
 */
int	MRcvedOpenMgr::del(GC_PRO &gcPro)
{
	MLocalSection					locSec;							

	gcPro.init();
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** 链表空*/
		return BUILD_ERROR(0, ELISTEMPTY);
 
 	RCVED_OPEN_NODE	*pNode = list_entry(m_stUsed.next, RCVED_OPEN_NODE, m_listNode);
	assert(pNode != NULL);
 
	/*fill data*/
	memcpy(&gcPro, &(pNode->m_stGcPro), sizeof(pNode->m_stGcPro));
 	
	/* 资源回归前重新初始化 */
	pNode->m_stGcPro.init();
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
			"从RCVDOPEN队列中关闭套接字[%d]",
			pNode->m_sock);
		MSocket::close(pNode->m_sock);
		pNode->m_sock = INVALID_SOCKET;
		list_del(pList);
		list_add_tail(pList, &m_stFree);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"从RCVDOPEN队列中共删除[%d]个套接字",
		total);
	}	
}


//////////////////////////////////////////////////////////////////////////


/**
 *	_MLOCALCONNECT_		-		构造函数，用以在线程中定义局部变量时的初如化
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
		pNode = list_entry(pTmp, MCONNECT_NODE, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"CONN线程退出后，将关闭套接字对[%d:%d]",
			pNode->m_stGcPro.m_sCliSock, pNode->m_stGcPro.m_sSrvSock);
		MSocket::close(pNode->m_stGcPro.m_sCliSock);
		MSocket::close(pNode->m_stGcPro.m_sSrvSock);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"CONN线程退出后,共释放[%d]对套接字",
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
int	_MLOCALCONNECT_::insert(GC_PRO &gcPro)
{
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ELINKOVERFLOW);
	
	MCONNECT_NODE	*pNode = list_entry(m_stFree.next, MCONNECT_NODE, m_listNode);
	memcpy(&(pNode->m_stGcPro), &gcPro, sizeof(gcPro));
	
	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
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
void _MLOCALCONNECT_::del(MCONNECT_NODE *pNode)
{
	assert(pNode != NULL);

	pNode->m_stGcPro.init();
	list_del(&(pNode->m_listNode));
	list_add_tail(&(pNode->m_listNode), &m_stFree);
}

/**
 *	isFull		-				是否满
 *
 *
 *	Note:						只在该模块的线程中使用，不用加锁
 *
 *	return
 *		!=0						已满
 *		==0						没满
 */
int _MLOCALCONNECT_::isFull()
{
	return list_empty(&m_stFree);
}

/**
 *	isEmpty		-				是否是空
 *
 *
 *	Note:						只在该模块的线程中使用，不用加锁
 *
 *	return
 *		!=0						已空
 *		==0						没空
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
 *	destroy		-			释放本层资源
 *
 *
 *	return
 *							无
 */
void MConnect::destroy()
{
	m_stRcvedOpenMgr.destroy();
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

	m_clAgeModule.waitExit(++err, --size);
}


/**
 *	_del		-				在mgr数据区取数据
 *
 *	@gcPro		[in/out]		取出的数据
 *
 *
 *	return
 *		==0						取成功
 *		!=0						取失败
 */
int	MConnect::_del(GC_PRO &gcPro)
{
	return m_stRcvedOpenMgr.del(gcPro);
}


/**
 *	insert		-				往数据区添加一个accept后的套接字
 *
 *	@gcPro			[in]		收到的open包中的协议信息
 *
 *
 *	return
 *		==0						添加成功
 *		!=0						添加失败
 */
int MConnect::insert(GC_PRO &gcPro)
{
	return m_stRcvedOpenMgr.insert(gcPro);
}

/**
 *	_doSucess		-			检索可读可写套接字，并执行相应操作
 *
 *	@local:			[in/out]	连接队列
 *
 *
 *	return
 *								无
 */
void	MConnect::_doSucess(GC_PRO &gcPro)
{
	int							rc;

	assert(gcPro.m_sSrvSock != INVALID_SOCKET);

	/* 设置服务端套接字属性*/
	MSocket::setSock(gcPro.m_sSrvSock, SOCK_RCV2SND_BUFF_SIZE);

	rc = m_clAgeModule.insert(gcPro);
	if(rc != 0)
		_doFailed(gcPro, rc);
}

void MConnect::_doFailed(GC_PRO &gcPro, int errCode)
{	
	Global_Log.writeLog(LOG_TYPE_ERROR, 
		"CONN线程以认证方式[%d]操作套接字对[%d,%d]失败[%d(%d:%d)],关闭该套接字对", 
		gcPro.m_eType, gcPro.m_sCliSock, gcPro.m_sSrvSock, errCode, GET_SYS_ERR(errCode), GET_USER_ERR(errCode));

	MSocket::close(gcPro.m_sCliSock);
	MSocket::close(gcPro.m_sSrvSock);
}

/**
 *	_getConnSrv		-			从协议信息中得到需要连接的服务器IP与端口
 *
 *	@gcPro:			[in]		协议数据
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int MConnect::_getConnSrv(const GC_PRO &gcPro)
{
	assert(gcPro.m_eType != DIRCONN);

	char				ip[256] = {0};
	int					rc;

	Global_Log.writeLog(LOG_TYPE_INFO,
		"套接字对[%d]要连接的代理服务器为[%s:%d], 代理类型[%d]",
		gcPro.m_sCliSock, gcPro.m_cAgIP, gcPro.m_usAgPort, gcPro.m_eType);

	/* dns转换 */
	rc = MSocket::dnsToIP((char *)&gcPro.m_cAgIP, sizeof(gcPro.m_cAgIP), ip, sizeof(ip)-1);
	if( rc != 0 ){
		Global_Log.writeLog(LOG_TYPE_WARN,
			"CONN中对[%s]进行DNS出错[%d]", 
			gcPro.m_cAgIP, rc);
		return rc;
	}
	
	/* 转化为网络字节序 */
	m_uiConnIP = inet_addr(ip);
	m_usConnPort = htons(gcPro.m_usAgPort);

	return 0;
}



/**
 *	_upConnSrv		-			更新连接的服务器IP与端口
 *
 *	@gcPro:			[in]		协议数据
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int MConnect::_upConnSrv( GC_PRO &gcPro)
{
	if(gcPro.m_eType == DIRCONN) /* 直连，是绿色通道的IP与端口 */
	{
		m_uiConnIP = inet_addr(gcPro.m_cGcSIP);
		m_usConnPort = htons(gcPro.m_usGcSPort);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"套接字对[%d]要连接的绿色通道服务器为[%s:%d]",
			gcPro.m_sCliSock, gcPro.m_cGcSIP, gcPro.m_usGcSPort);

		return 0;
	}else
	{
		return _getConnSrv(gcPro);
	}
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
		/* 本地没满 */
		if(!local.isFull())
		{
			if(pObj->_del(gcPro) == 0)/* 取出数据 */
			{
				assert(gcPro.m_sCliSock != INVALID_SOCKET && gcPro.m_sSrvSock == INVALID_SOCKET);
				/* 更新要连接的服务器IP与端口 */
				rc = pObj->_upConnSrv(gcPro);
				if(rc != 0){
					Global_Log.writeLog(LOG_TYPE_WARN, 
						"套接字更新连接的服务器失败,关闭该套接字", 
						gcPro.m_sCliSock, gcPro.m_sCliSock);
					MSocket::close(gcPro.m_sCliSock);
					continue;
				}

				rc = MSocket::asynConnect(pObj->m_uiConnIP, pObj->m_usConnPort, &srvSock);
				gcPro.m_sSrvSock = srvSock;	/* 填充服务端套接字 */
				if(rc < 0)
				{
					pObj->_doFailed(gcPro,  BUILD_ERROR(_OSerrno(), ECONNECT));
				}else if(rc == 0)/* 直接成功，理论上很少发生*/
				{
					pObj->_doSucess(gcPro);
				}else	/* 需要select来检测成功*/
				{
					local.insert(gcPro);
				}
			}
		}
		
		if(!local.isEmpty())
		{
			FD_ZERO(&fdWrite);
			/** 把所有套接字加入*/
			list_for_each(pList, &local.m_stUsed)
			{
				pNode = list_entry(pList, MCONNECT_NODE, m_listNode);
				FD_SET(pNode->m_stGcPro.m_sSrvSock, &fdWrite);
			}

			rc = select(0, NULL, &fdWrite, NULL, &tv);
			if(rc > 0)	/** 成功*/
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
				if(rc < 0)/* 发生错误 */
					Global_Log.writeLog(LOG_TYPE_ERROR, "CONN线程select出错[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
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

	/*  仅用户取消才会退出 */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"CONN线程退出");

	return 0;
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


