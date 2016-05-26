#include <windows.h>
#include <assert.h>
#include "MHand.h"
#include "MGlobal.h"
#include "MList.h"
#include "MSocket.h"
#include "gcCliInterface.h"

/*
 *_MLOCALHAND_	-	构造函数，在线程中定义局部变量时的初始化
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
 *~_MLOCALHAND_	-	析构函数，在Hand线程退出后，完成套接字对的关闭
 */
_MLOCALHAND_::~_MLOCALHAND_()
{
	struct list_head		*pTmp;
	HAND_PEER				*pNode;
	int						total = 0;/* 存储遗留的套接字对，方便查看问题 */
	
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, HAND_PEER, m_List);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"HAND线程退出,套接字对状态为[%d],将关闭套接字对[%d:%d],",
			pNode->m_peerState,
			pNode->m_cliSock, pNode->m_srvSock);
		MSocket::close(pNode->m_cliSock);
		MSocket::close(pNode->m_srvSock);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"HAND线程退出后，共释放[%d]对套接字",
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
 *	destroy		-				释放资源,不用加锁(已经没人在用这个了)
 *
 *
 *	return
 *								无
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
			"从connected队列中关闭套接字对[%d]",
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
			"从connected队列中共删除[%d]个套接字对",
		total);
	}	
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
int	MConnedMgr::insert(SOCKET cliSock, SOCKET srvSock)
{	
	MLocalSection			locSec;
	
	locSec.Attch(&m_Lock);

	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, EHANDFULL);

	/* 设置套接字属性 */
	_setSock(cliSock);
	_setSock(srvSock);
	
	/** fill sock*/
	CONN_NODE	*pNode = list_entry(m_stFree.next, CONN_NODE, m_lListNode);
	pNode->m_sCliSock = cliSock;
	pNode->m_sSrvSock = srvSock;

	Global_Log.writeLog(LOG_TYPE_INFO,
		"套接字对[%d:%d]完成连接",
		cliSock, srvSock);	
	return MList::_del2insert(&(pNode->m_lListNode), &m_stUsed);
}

/**
 *	_setSock		-			设置套接字的属性(非阻塞，发送与接收缓冲区大小)
 *
 *	@sock			[in]		需要设置的套接字
 *
 *	return
 *								无
 */
void MConnedMgr::_setSock(SOCKET sock)
{
	assert(sock != INVALID_SOCKET);

	if(MSocket::setnonblocking(sock) != 0)
		Global_Log.writeLog(LOG_TYPE_WARN,
			"设置套接字[%d]为非阻塞出错", sock);

	if(MSocket::setSockBuf(sock, SOCK_RCV2SND_BUFF_SIZE) != 0)
		Global_Log.writeLog(LOG_TYPE_WARN,
			"设置套接字[%d]缓冲区(发送与接收)出错", sock);
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
int	MConnedMgr::del(SOCKET &cliSock, SOCKET &srvSock)
{
	MLocalSection					locSec;							
	
	cliSock = INVALID_SOCKET;
	srvSock = INVALID_SOCKET;
	
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** 链表空*/
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
void	MHand::waitExit(int *err, size_t size)
{
	int					rc = 0;
	
	if(m_hHandHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_hHandHandle, WAIT_THREAD_TIMEOUT);
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待HAND线程退出超时[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待HAND线程退出出错[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}

		m_hHandHandle = 0;
	}
	
	/* 释放本层资源 */
	destroy();
	/* 填充本层错误码 */
	err[0] = rc;
	assert(size > 1);

	/* 等待下层返回 */
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
 *	destroy		-				释放本层资源
 *
 *	return
 *								无
 */
void MHand::destroy()
{
	m_clConnetedMgr.destroy();
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
int	MHand::insert(SOCKET cliSock, SOCKET srvSock)
{
	return m_clConnetedMgr.insert(cliSock, srvSock);
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
int	MHand::del(SOCKET &cliSock, SOCKET &srvSock)
{
	return m_clConnetedMgr.del(cliSock, srvSock);
}

/**
 *	handThread		-			握手线程的执行函数，负责握手
 *
 *	@accSock:		[in]		MHand对象指针
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
		/* 本地hand链表还没满 */
		if(!list_empty(&localHand.m_stFree))
		{
			/* 试图加一个已经连接好的套接字对 */
			if(pObj->m_clConnetedMgr.del(cliSock, srvSock) == 0)
			{
				assert(cliSock != INVALID_SOCKET);
				assert(srvSock != INVALID_SOCKET);
				/* 加到本地hand链表中 */
				pObj->_insertToLocal(localHand, cliSock, srvSock, RCVING_HELLO);
			}
		}

		if(!list_empty(&localHand.m_stUsed))
		{
			FD_ZERO(&fdRead);
			/* 遍历本地链表，添加进select*/
			list_for_each(pTmp, &localHand.m_stUsed)
			{
				pNode = list_entry(pTmp, HAND_PEER, m_List);
				assert(pNode != NULL);
				if(pNode->m_peerState == RCVING_HELLO || 
					pNode->m_peerState == RCVING_UNIT_FROM_SRV){ /* 从SRV收HELLO包或握手包响应*/
					FD_SET(pNode->m_srvSock, &fdRead);
				}else if(pNode->m_peerState == RCVING_UNIT_FROM_CLI){	/* 从CLI收握手包*/
					FD_SET(pNode->m_cliSock, &fdRead);
				}else{ assert(0); }
			}
			
			rc = select(0, &fdRead, NULL, NULL, &tv);
			if(rc > 0)/* 检测成功*/
			{
				list_for_each_safe(pTmp, pSafe, &localHand.m_stUsed)
				{
					pNode = list_entry(pTmp, HAND_PEER, m_List);
					assert(pNode != NULL);
					if(FD_ISSET(pNode->m_cliSock, &fdRead)) {/* 客户端有数据*/
						_rc = pObj->_doRcv(pNode, false);
					}else if(FD_ISSET(pNode->m_srvSock, &fdRead)) {/* 服务端有回应*/
						_rc = pObj->_doRcv(pNode, true);
					}
					if(_rc != 0){ /* 测试点 */
						pObj->_doFailed(localHand, pNode, _rc);
						pObj->_pull(pNode, &localHand.m_stFree);
					}else if(pNode->m_peerState == TRANSFORMING){
						pObj->_doSucess(localHand, pNode);
						pObj->_pull(pNode, &localHand.m_stFree);
					}				
				}
			}else
			{
				if(rc < 0){/* 发生错误*/
					Global_Log.writeLog(LOG_TYPE_ERROR, "HAND线程select出错[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
					assert(0);
				}
				
			}
		}else{Sleep(15); }/* 防止CPU过高 */
	}while(Global_IsRun);

	/* 仅当用户取消才会返回 */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"HAND线程退出");

	return 0;
}

/**
 *	_pull		-			归还资源，并相应的初始化
 *
 *	@pNode			[in]		要删除的节点
 *	@head			[in]		回到哪个链表中
 *
 *	return
 *					无
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
 *	_doFailed		-			握手失败，删除套接字对，并归还资源
 *
 *	@localHand		[in/out]	本地握手数据管理
 *	@peer			[in]		需要关闭的套接字对
 *	@errCode		[in]		套接字对出错原因
 *
 *	return
 *					无
 */
void MHand::_doFailed(MLOCALHAND &localHand, HAND_PEER *peer, int errCode)
{
	assert(peer != NULL);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"握手协议发生错误[%d(%d:%d)]状态为[%d], 关闭套接字对[%d:%d]",
		errCode, GET_SYS_ERR(errCode),GET_USER_ERR(errCode),peer->m_peerState,
		peer->m_cliSock, peer->m_srvSock);

	MSocket::close(peer->m_cliSock);
	MSocket::close(peer->m_srvSock);
}

/**
 *	_doSucess		-			握手完成，向下级添加元素
 *
 *	@localHand		[in/out]	本地握手数据管理
 *	@peeer			[in]		握手完成的套接字对
 *
 *
 *	return
 *					无
 */
void MHand::_doSucess(MLOCALHAND &localHand, HAND_PEER *peer)
{
	int						rc;

	assert(peer != NULL && peer->m_peerState == TRANSFORMING);
	assert(peer->m_cliSock != INVALID_SOCKET && peer->m_srvSock != INVALID_SOCKET);
	
	rc = m_clTranModule.insert(peer->m_cliSock, peer->m_srvSock);
	if(rc != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
		"HAND线程向TRANS线程添加套接字对发生错误[%d]，关闭套接字对[%d:%d]",
		rc, peer->m_cliSock, peer->m_srvSock);
		MSocket::close(peer->m_cliSock);
		MSocket::close(peer->m_srvSock);
	}	
}


/**
 *	_rcvHello		-			收hello包
 *
 *	@peer			[in/out]	套接字对
 *
 *
 *	return
 *		==0						收全成功
 *		!=0						失败
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

	rc = MSocket::rcvDataNonBlock(peer->m_srvSock, buffer, _rc); /** 测试点*/ 
	if(rc == 0)/* 成功*/
		peer->m_peerState = RCVING_UNIT_FROM_CLI;

	assert(memcmp(buffer, buffer2, _rc) == 0);
	if(memcmp(buffer, buffer2, _rc) != 0)
		return BUILD_ERROR(0, EHELLO);

	return rc;
}




/**
 *	_insertToLocal		-		加到本地hand链表中
 *
 *	@local			[in/out]	本地hand结构
 *	@cliSock:		[in]		客户端套接字
 *	@srvSock:		[in]		服务端套接字
 *	@state			[in]		该套接字对的状态
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
 *	_rcvUnitFromSrv		-		向SRV收握手回应包，并回应客户端
 *
 *	@peer			[in/out]	套接字对
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败，上级需要关闭套接字对
 */
int	MHand::_rcvUnitFromSrv(HAND_PEER *peer)
{
	char					rcvBuff[1024];
	char					commUnit[1024];
	char					tradeUnit[1024];
	int						rc,_rc;

	/* 得到应该收包的大小(普通unit和委托unit都一样) */
	rc = _snprintf(commUnit, sizeof(commUnit),
		HTTP_UNIT_FMT_DOWN, GCA_OPEN, 0, 0, 0);
	assert(rc <= sizeof(rcvBuff));

	rc = _snprintf(tradeUnit, sizeof(commUnit),
		HTTP_UNIT_FMT_DOWN, GCA_OPEN2, 0, 0, 0);
	assert(rc <= sizeof(rcvBuff));

	_rc = rc;	/* 缓存应收大小 */

	rc = MSocket::rcvDataNonBlock(peer->m_srvSock, rcvBuff, _rc);
	if(rc != 0)
		return rc;

	/* 校验收到的包内容(包括委托与非委托) */
	if((memcmp(rcvBuff, commUnit, _rc) != 0)
		&&(memcmp(rcvBuff, tradeUnit, _rc) != 0))
	{
#define ERRCODE	"GCErCD:"
		char *pErrCode = _StrStr(rcvBuff, sizeof(rcvBuff)-1, ERRCODE);
		int	 errCode = atoi(pErrCode+sizeof(ERRCODE));
		Global_Log.writeLog(LOG_TYPE_WARN, 
			"握手协议服务器返回错误码[%d]", errCode);
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
 *	_rcvUnitFromCli		-		向CLI收握手包，并上报服务
 器
 *
 *	@peer			[in/out]	套接字对
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败，上级需要关闭套接字对
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
	if(pPro->ucApplyType == 1) {/* 普通的握手包 */
		rc = sprintf(sndBuffer, HTTP_UNIT_FMT_UP,
			pCer->strIPAddress, pCer->usPort,
			GCA_OPEN, 0, 0, 0);	/* 测试点: 都填的0 */
	}else if(pPro->ucApplyType == 2) {/* 委托的握手包 */	
		rc = sprintf(sndBuffer, HTTP_UNIT_FMT_UP,
			pCer->strIPAddress, pCer->usPort,
			GCA_OPEN2, 0, 0, 0);/* 测试点: 都填的0 */
	}else{
		Global_Log.writeLog(LOG_TYPE_ERROR,
			"客户端发上来的握手包类型[%d]出错",
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
 *	_doRcv			-			收相应协议并处理相应协议
 *
 *	@peer			[in/out]	处理的套接字对
 *	@isSrv			[in]		该套接字对是否是SRV
 *
 *	return
 *		==0						协议处理成功
 *		!=0						协议处理失败，该套接字对应该关闭
 */
int MHand::_doRcv(HAND_PEER *peer, bool isSrv)
{
	assert(peer != NULL);
	assert(peer->m_cliSock != INVALID_SOCKET);
	assert(peer->m_srvSock != INVALID_SOCKET);

	if(isSrv)/* 服务端 */
	{
		if(peer->m_peerState == RCVING_HELLO)
			return _rcvHello(peer);
		else if(peer->m_peerState == RCVING_UNIT_FROM_SRV)
			return _rcvUnitFromSrv(peer);
		else{
			Global_Log.writeLog(LOG_TYPE_ERROR,
				"状态有错，在[%d]端，此时的状态为[%d]",
				isSrv, peer->m_peerState);
			assert(0);
			return BUILD_ERROR(0, ESTATUS);
		}
	}else/* 客户端 */
	{
		if(peer->m_peerState == RCVING_UNIT_FROM_CLI)
			return _rcvUnitFromCli(peer);
		else{
			Global_Log.writeLog(LOG_TYPE_ERROR,
				"状态有错，在[%d]端，此时的状态为[%d]",
				isSrv, peer->m_peerState);
			assert(0);
			return BUILD_ERROR(0, ESTATUS);
		}
	}
}
