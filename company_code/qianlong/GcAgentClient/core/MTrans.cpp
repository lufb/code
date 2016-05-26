#include <assert.h>
#include "MTrans.h"
#include "MLocalLock.h"
#include "MList.h"
#include "MGlobal.h"
#include "MHttpParase.h"
#include "MSocket.h"

/*
 *	_MLOCALTRANS_	-	构造函数，在线程中创建该局部变量时的初始化
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
 *	initTransNode		-		初始化一个TRANS_SOCKET_PEER节点
 *
 *	@pNode			[in/out]	需要初始化的节点
 *
 *
 *	return
 *								无
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
 *	~_MLOCALTRANS_	-	析构函数，在线程退出后关闭遗留套接字
 */
_MLOCALTRANS_::~_MLOCALTRANS_()
{
	struct list_head		*pTmp;
	TRANS_SOCKET_PEER		*pNode;
	int						total = 0;/* 存储遗留的套接字对，方便查看问题 */

	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, TRANS_SOCKET_PEER, m_lListNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"TRANS线程退出，将关闭套接字对[%d:%d],共转发字节数[%d:%d], 遗留字节数[%d:%d]",
			pNode->m_cli.m_sock, pNode->m_srv.m_sock,
			pNode->m_cli.m_tranSize, pNode->m_srv.m_tranSize,
			pNode->m_cli.m_engine.leaveParased, pNode->m_srv.m_engine.leaveParased);
		MSocket::close(pNode->m_cli.m_sock);
		MSocket::close(pNode->m_srv.m_sock);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"TRANS线程退出后，共释放[%d]个套接字对",
		total);
	}	
}

/**
 *	insert		-				添加数据
 *
 *	@cliSock:		[in]		客户端套接字
 *	@srvSock:		[in]		服务端套接字
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int	_MLOCALTRANS_::insert(SOCKET cliSock, SOCKET srvSock)
{
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, ELINKOVERFLOW);
	
	TRANS_SOCKET_PEER	*pNode = list_entry(m_stFree.next, TRANS_SOCKET_PEER, m_lListNode);
	/* 不用初始化节点信息:从free得到的节点保证是经过初始化的 */
	/* 只需直接填充两端套接字 */
	pNode->m_cli.m_sock = cliSock;
	pNode->m_srv.m_sock = srvSock;
	
	/* 将该节点从m_stFree加到m_stUsed链表中 */
	return MList::_del2insert(&(pNode->m_lListNode), &m_stUsed);
}

/**
 *	del		-					删除数据，并做相应初始化
 *
 *	@pNode			[in]		需要删除的节点
 *
 *
 *	return
 *								无
 */
void _MLOCALTRANS_::del(TRANS_SOCKET_PEER *pNode)
{
	/* 重新初始化该节点 */
	initTransNode(pNode);

	/* 资源回归 */
	list_del(&(pNode->m_lListNode));
	list_add_tail(&(pNode->m_lListNode), &m_stFree);
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
int _MLOCALTRANS_::isFull()
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
 *	delHanded		-			清空handed队列
 *
 *
 *	return
 *								无
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
			"从handed队列中关闭套接字对[%d,%d]",
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
			"从handed队列中共删除[%d]对套接字",
		total);
	}
}

/**
 *	destroy		-				释放资源,不用加锁(已经没人在用这个了)
 *
 *
 *	return
 *								无
 */
void MHandedMgr::destroy()
{
	delHanded();
}
/**
 *	insert		-		添加元素
 *
 *	@cliSock					客户端套接字
 *	@srvSock					服务端套接字
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
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
		"套接字对[%d:%d]完成握手",
		cliSock, srvSock);
	
	return MList::_del2insert(&(pNode->m_lListNode), &m_stUsed);
}

/**
 *	del		-					取元素
 *
 *	@node						取出的元素
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MHandedMgr::del(SOCKET &cliSock, SOCKET &srvSock)
{
	MLocalSection					locSec;							
	
	cliSock = INVALID_SOCKET;
	srvSock = INVALID_SOCKET;
	
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** 链表空*/
		return BUILD_ERROR(0, ELISTEMPTY);
	
	HANDED_NODE	*pNode = list_entry(m_stUsed.next, HANDED_NODE, m_lListNode);
	cliSock = pNode->m_sCliSock;
	srvSock = pNode->m_sSrvSock;
	
	/* 资源回归前的初始化 */
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
 *	destroy		-				释放本层资源
 *
 *
 *	return
 *								无
 */
void MTrans::destroy()
{
	m_clHandedMgr.destroy();
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
void	MTrans::waitExit(int *err, size_t size)
{
	int					rc = 0;

	if(m_hTraHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_hTraHandle, WAIT_THREAD_TIMEOUT);
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待TRANS线程退出超时[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待TRANS线程退出出错[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}
		m_hTraHandle = 0;
	}

	/* 释放本层资源 */
	destroy();

	/* 填充本层错误码 */
	err[0] = rc;

	/* 已无下层 */
}

/**
 *	insert		-			添加元素
 *
 *	@cliSock					客户端套接字
 *	@srvSock					服务端套接字
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MTrans::insert(SOCKET cliSock, SOCKET srvSock)
{
	return m_clHandedMgr.insert(cliSock, srvSock);
}

/**
 *	del		-					取元素
 *
 *	@node						取出的元素
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MTrans::del(SOCKET &cliSock, SOCKET &srvSock)
{
	return m_clHandedMgr.del(cliSock, srvSock);
}

unsigned int __stdcall MTrans::transThread(void * in)
{
	assert(NULL != in);
	MTrans					*pObj = (MTrans*)in;
	MLOCALTRANS				local;				/* 局部变量,这个局部变量稍微有点大，应该在创建线程时，把栈设大些*/
	fd_set					fdRead;
	SOCKET					cliSock, srvSock;
	struct timeval			tv;
	struct list_head		*pTmp, *pSafe;
	TRANS_SOCKET_PEER		*pNode;
	int						rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);

	do{
		/** 本地trans链表还没满*/
		if(!local.isFull())
		{
			/* 试图加一个已经握手完成的套接字对*/
			if(pObj->m_clHandedMgr.del(cliSock, srvSock) == 0){
				assert(cliSock != INVALID_SOCKET && srvSock != INVALID_SOCKET);
				local.insert(cliSock, srvSock);		/* 添加肯定是成功的，不用校验返回值 */
			}
		}else{
			Global_Log.writeLog(LOG_TYPE_WARN,
				"TRANS线程中，套接字对缓冲区已满，开始关闭handed队列");
			pObj->m_clHandedMgr.delHanded();
		}

		if(!local.isEmpty())
		{
			/* 有需要检测的套接字 */
			FD_ZERO(&fdRead);
			/* 遍历本地链表，添加select元素 */
			list_for_each(pTmp, &local.m_stUsed)
			{
				pNode = list_entry(pTmp, TRANS_SOCKET_PEER, m_lListNode);
				assert(pNode != NULL && pNode->m_cli.m_sock != INVALID_SOCKET && pNode->m_srv.m_sock != INVALID_SOCKET);
				/* 把套接字对加入检测 */
				FD_SET(pNode->m_cli.m_sock, &fdRead);
				FD_SET(pNode->m_srv.m_sock, &fdRead);
			}

			rc = select(0, &fdRead, NULL, NULL, &tv);
			if(rc > 0){ /* 检测到套接字 */
				list_for_each_safe(pTmp, pSafe, &local.m_stUsed)
				{
 					pNode = list_entry(pTmp, TRANS_SOCKET_PEER, m_lListNode);
					if(FD_ISSET(pNode->m_cli.m_sock, &fdRead))	/* CLI -> SRV */
						if(pObj->_trans(local, pNode, false) != 0)
							continue;	/* 提前终止 */
					if(FD_ISSET(pNode->m_srv.m_sock, &fdRead))
						pObj->_trans(local, pNode, true); /* SRV -> CLI */
				}
			}else{
				if(rc < 0){ /* 发生错误 */
					Global_Log.writeLog(LOG_TYPE_ERROR,
					"TRANS线程中select发生错误[%d]",
					BUILD_ERROR(_OSerrno(), EABORT));
					assert(0);
				}
			}
		}else{Sleep(15);} /* 防止CPU过高 */
	}while(Global_IsRun);

	/* 仅用户取消才退出 */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"TRANS线程退出");
	
	return 0;
}

/**
 *	_trans		-				数据转发,如果转发出错(有链路关闭)，则关闭该套接字对，并归还资源
 *
 *	@local			[in/out]	转发线程用到的局部对象对列
 *	@pNode			[in]		转发用到的套接字对
 *	@isSrv			[in]		转发的方向
 *
 *
 *	return
 *		==0						成功转发
 *		!=0						错误码 error.h
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
			"TRANS转发时返回[%d(%d:%d)],将关闭套接字对[%d:%d],共转发字节数[%u,%u],遗留字节数[%u, %u]",
			rc, GET_SYS_ERR(rc), GET_USER_ERR(rc),
			pNode->m_cli.m_sock, pNode->m_srv.m_sock,
			pNode->m_cli.m_tranSize, pNode->m_srv.m_tranSize,
			pNode->m_cli.m_engine.leaveParased, pNode->m_srv.m_engine.leaveParased);

		assert(pNode->m_cli.m_engine.leaveParased == 0);
		assert(pNode->m_srv.m_engine.leaveParased == 0);

		MSocket::close(pNode->m_cli.m_sock);
		MSocket::close(pNode->m_srv.m_sock);

		local.del(pNode);/* 资源回归 */		
	}

	return rc;
}

/**
 *	_srvToCli		-			SRV到CLI的数据转发
 *
 *	@peer			[in/out]	套接字对信息
 *
 *
 *	return
 *		==0						成功转发
 *		!=0						上级关闭该套接字对
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
 *	_cliToSrv		-			CLI到SRV的数据转发
 *
 *	@peer			[in/out]	套接字对信息
 *
 *
 *	return
 *		==0						成功转发
 *		!=0						上级关闭该套接字对
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
	peer->m_cli.m_engine.leaveParased += _rc;	/*遗留字节数累加*/

	rc = MHttpParase::buildHttpData(peer->m_cli.m_sockBuffer+MAX_HTTPHEAD_SIZE,
		rc,
		MAX_HTTPHEAD_SIZE,
		&pSend,
		0,
		0); /* 测试点:固定填0是否有缺陷 */

	if(rc < 0)
		return rc;

	if(send(peer->m_srv.m_sock, pSend, rc, 0) != rc)
		return BUILD_ERROR(_OSerrno(), ESCLOSE);

	/* 统计转发数据*/
	peer->m_cli.m_tranSize += _rc;
	/*遗留字节数累减*/
	peer->m_cli.m_engine.leaveParased -= _rc;

	return 0;
}



