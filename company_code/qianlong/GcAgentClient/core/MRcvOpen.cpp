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
		return BUILD_ERROR(0, ELINKOVERFLOW);

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
int	MAcceptMgr::del(SOCKET &sock)
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

//////////////////////////////////////////////////////////////////////////



/**
 *	_MLOCALRCVOPEN_		-		构造函数，用以在线程中定义局部变量时的初如化
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
 *	~_MLOCALRCVOPEN_		-		析构函数，线程退出后，该局部变量的析构来完成套接字的关闭
 */
_MLOCALRCVOPEN_::~_MLOCALRCVOPEN_()
{
	struct list_head	*pTmp;
	MRCVOPEN_NODE		*pNode;
	int					total = 0;	/* 存储遗留的套接字对，方便查看问题 */
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, MRCVOPEN_NODE, m_lListNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"RCVOPEN线程退出后，将关闭套接字[%d]",
			pNode->m_sSock);
		MSocket::close(pNode->m_sSock);
	}

	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"RCVOPEN线程退出时,还有[%d]个套接字在等待用户发送认证信息",
		total);
	}	
}

/**
 *	insert		-				添加数据
 *
 *	@sock			[in]		添加的套接字		
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
 *	del		-					归还资源，并相应的初始化
 *
 *	@pNode			[in]		要删除的节点
 *
 *	return
 *					无
 */
void _MLOCALRCVOPEN_::del(MRCVOPEN_NODE *pNode)
{
	pNode->m_sSock = INVALID_SOCKET;
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
int _MLOCALRCVOPEN_::isFull()
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
void	MRcvOpen::waitExit(int *err, size_t size)
{
	int						rc = 0;

	if(m_hRcvOpenHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_hRcvOpenHandle, WAIT_THREAD_TIMEOUT);				
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待RCVOPEN线程退出超时[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待RCVOPEN线程退出出错[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}

		m_hRcvOpenHandle = 0;
	}
	
	/* 释放本层资源 */
	destroy();
	/* 填充本层错误码 */
	err[0] = rc;
	assert(size > 1);

	m_clConnModule.waitExit(++err, --size);
}

/**
 *	destroy		-			释放本层资源
 *
 *
 *	return
 *							无
 */
void MRcvOpen::destroy()
{
	m_stAcceptedMgr.destroy();
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
int MRcvOpen::insert(SOCKET accSock)
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
int	MRcvOpen::_del(SOCKET &sock)
{
	return m_stAcceptedMgr.del(sock);
}


/**
 *	_fillType		-			根据open包填充类型和代理类型(委托/非委托)
 *
 *	@rcvPro			[in]		收到协议数据(头+体)
 *	@size			[in]		收到的协议数据长度
 *	@gcPro			[in/out]	填充本地协议结构体
 *
 *	Note:
 *					传入对数size：
 *								已由上层调用决定只可能是新协议数据体的长度或老协议数据体的长度
 *
 *	return
 *		==0						成功
 *		!=0						失败 对应error.h错误码
 */
int MRcvOpen::_fillType(const char *rcvPro, const size_t size, GC_PRO &gcPro)
{
	assert(NULL != rcvPro);
	tagQLGCProxy_Apply		*pApply = (tagQLGCProxy_Apply*)rcvPro;
	unsigned char			applyType;
	int						rc = 0;			/* 返回值 */

	applyType = pApply->ucApplyType;
	if(applyType != 1 && applyType != 2)/* 只可能是这两种(非委托与委托)情况 */
		return BUILD_ERROR(0, EUSERPROTOCAL);

	gcPro.m_ucApplyType = applyType;	/* 填充类型(委托/非委托)*/

	if(size == sizeof(tagQLGCProxy_Certify) + sizeof(tagQLGCProxy_Apply)){/* 老协议 */
		gcPro.m_eType = DIRCONN;	/* 老协议只能是直连 */
		return 0;
	} else if(size == sizeof(tagNewQLGCProxy_Certify) + sizeof(tagQLGCProxy_Apply) ){/* 新协议 */
		tagNewQLGCProxy_Certify			*pPro = (tagNewQLGCProxy_Certify *)(rcvPro+sizeof(tagQLGCProxy_Apply));
		
		if(memcmp(pPro->strMagic, GCC_PROXY_HEAD, strlen(GCC_PROXY_HEAD)))
			return BUILD_ERROR(0, E_ERR_MAAGIC);

		switch (pPro->ucType)		/* 根据协议信息填充连接方式 */
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
			gcPro.m_eType = INVALID_REG;/* 置为非法类型 */
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
 *	_fillPro		-			根据open包填充本地协议数据
 *
 *	@rcvPro			[in]		收到协议数据
 *	@size			[in]		收到的协议数据长度
 *	@gcPro			[in/out]	填充本地协议结构体
 *
 *	Note:
 *					传入对数size：
 *								已由上层调用决定只可能是新协议数据体的长度或老协议数据体的长度
 *
 *	return
 *		==0						成功
 *		!=0						失败 对应error.h错误码
 */
int	MRcvOpen::_fillPro(const char *rcvPro, const size_t size, GC_PRO &gcPro)
{
	/* 先填充绿色通道的IP与端口 */
	Global_Option.getGcSIPPort(gcPro.m_cGcSIP, sizeof(gcPro.m_cGcSIP), gcPro.m_usGcSPort);

	if(size == sizeof(tagQLGCProxy_Certify))				/* 老协议填充方式 */
	{
		tagQLGCProxy_Certify	*pOldPro = (tagQLGCProxy_Certify *)rcvPro;

		strncpy(gcPro.m_cDstIP, pOldPro->strIPAddress, sizeof(gcPro.m_cDstIP)-1);
		gcPro.m_usDstPort = pOldPro->usPort;
#ifdef _DEBUG
		/* 打印用户的参数到日志，以方便调试 */
		Global_Log.writeLog(LOG_TYPE_INFO,
			"Old Protocol,strIPAddress[%s],usPort[%d], CheckCode[%d]",
			pOldPro->strIPAddress, pOldPro->usPort, pOldPro->CheckCode);
#endif
		return 0;
	}else if(size == sizeof(tagNewQLGCProxy_Certify))	/* 新协议填充方式 */
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
		/* 打印用户的参数到日志，以方便调试 */
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
 *	_doRcvOpen		-			收用户的连接信息
 *
 *	@pNode:			[in]		节点信息
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int MRcvOpen::_doRcvOpen(MRCVOPEN_NODE *pNode)
{
	assert(pNode != NULL && pNode->m_sSock != INVALID_SOCKET);
	char					rcvBuffer[1024];	/* 收协议数据缓冲区 1024是足够的 */
	GC_PRO					gcPro;				/* 需要填充的本地协议信息 */
	unsigned short			bodySize;
	int						rc;

	/* step1: 收协议头*/
	rc = MSocket::rcvDataNonBlock(pNode->m_sSock, rcvBuffer, 
		sizeof(tagQLGCProxy_Apply));
	if(rc != 0)
		return rc;

	/* stpe2:校验头 */
	if(memcmp(rcvBuffer, USER_DATA_HEAD, strlen(USER_DATA_HEAD)) != 0)
		return BUILD_ERROR(0, EUSERPROTOCAL);
	
	/* step3: 得到数据体的长度*/
	bodySize = ((tagQLGCProxy_Apply*)rcvBuffer)->usSize;
	if(bodySize == 0)
		return BUILD_ERROR(0, EUSERPROTOCAL);

	/* step4: 收体大小 */
	rc = MSocket::rcvDataNonBlock(pNode->m_sSock, 
		rcvBuffer+sizeof(tagQLGCProxy_Apply), bodySize);
	if(rc != 0)
		return rc;

	gcPro.init();

	/* 填充协议类型和代理类型(委托/非委托) */
	if((rc = _fillType(rcvBuffer, bodySize+sizeof(tagQLGCProxy_Apply), gcPro)) != 0)
		return rc;

	/* 填充本地协议数据 */
	if((rc = _fillPro(rcvBuffer+sizeof(tagQLGCProxy_Apply), bodySize, gcPro)) != 0)
		return rc;

	/* 填充客户端套接字 */
	gcPro.m_sCliSock = pNode->m_sSock;

	/* 往下层添加 */
	return m_clConnModule.insert(gcPro);
}




/**
 *	rcvOpenThread		-		收用户的连接信息函数
 *
 *	@accSock:		[in]		MConnect对象指针
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
unsigned int __stdcall MRcvOpen::rcvOpenThread(void * in)
{
 	MRcvOpen				*pObj = (MRcvOpen* )in;
	struct timeval			tv;						/** the timeout of select*/
	MLOCALRCVOPEN			local;					/* 该线程使用的局部变量*/
	fd_set					fdRead;					/* 可读套接字集合*/
	SOCKET					sock;					/* 临时套接字缓冲*/
	struct list_head		*pList, *pSafe;
	MRCVOPEN_NODE			*pNode;
	int						rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);
	
	do{
		/** 如果没有满，试图添加accept的套接字进来*/
		if(!local.isFull())
		{
			if(pObj->_del(sock) == 0)	/* 取accept的套接字成功 */
			{
				assert(sock != INVALID_SOCKET);
				local.insert(sock); /* 这里肯定添加成功，不用校验返回值 */
			}
		}

		/* 如果本地中有套接字 */
		if(!local.isEmpty())
		{
			FD_ZERO(&fdRead);
			/** 把所有套接字加入select*/
			list_for_each(pList, &local.m_stUsed)
			{
				pNode = list_entry(pList, MRCVOPEN_NODE, m_lListNode); assert(pNode != NULL);
				FD_SET(pNode->m_sSock, &fdRead);
			}
			
			rc = select(0, NULL, &fdRead, NULL, &tv);
			if(rc > 0)	/* 有套接字可读 */
			{
				list_for_each_safe(pList, pSafe, &local.m_stUsed)
				{
					pNode = list_entry(pList, MRCVOPEN_NODE, m_lListNode); assert(pNode != NULL);
					if(FD_ISSET(pNode->m_sSock, &fdRead))
					{
						rc = pObj->_doRcvOpen(pNode);
						if(rc != 0){
							Global_Log.writeLog(LOG_TYPE_ERROR,
								"套接字[%d]RCVOPEN出错[%d], 关闭套接字[%d]", 
								pNode->m_sSock, rc, pNode->m_sSock);
							MSocket::close(pNode->m_sSock);
						}else{
							Global_Log.writeLog(LOG_TYPE_INFO,
								"套接字[%d]已完成RCVOPEN",
								pNode->m_sSock);
						}
						/* 资源回归 */
						local.del(pNode);
					}
				}
			}else if(rc < 0)
			{
				assert(0);
				Global_Log.writeLog(LOG_TYPE_ERROR, "RCVOPEN线程select出错[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
			}
		}else{ Sleep(15);}
	}while(Global_IsRun);

	/*  仅用户取消才会退出 */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"RCVOPEN线程退出");

	return 0;
}