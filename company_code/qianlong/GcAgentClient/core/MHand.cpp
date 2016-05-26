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
	m_cApplyType = 0;					/* 0是非法的，1 普通 2 委托 */
	memset(m_cDstIP, 0, sizeof(m_cDstIP));
	m_usDstPort = 0;
}
/////////////////////////////
/*
 *_MLOCALHAND_	-	构造函数，在线程中定义局部变量时的初始化
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
 *~_MLOCALHAND_	-	析构函数，在Hand线程退出后，完成套接字对的关闭
 */
_MLOCALHAND_::~_MLOCALHAND_()
{
	struct list_head		*pTmp;
	HAND_LIST_NODE			*pNode;
	int						total = 0;/* 存储遗留的套接字对，方便查看问题 */
	
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, HAND_LIST_NODE, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"HAND线程退出,套接字对状态为[%d],将关闭套接字对[%d:%d],",
			pNode->m_peerState,
			pNode->m_stAgentedNode.m_sCliSock, pNode->m_stAgentedNode.m_sSrvSock);
		MSocket::close(pNode->m_stAgentedNode.m_sCliSock);
		MSocket::close(pNode->m_stAgentedNode.m_sSrvSock);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"HAND线程退出后，共释放[%d]对套接字",
			total);
	}
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
int _MLOCALHAND_::isFull()
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
int	_MLOCALHAND_::isEmpty()
{
	return list_empty(&m_stUsed);
}

/**
 *	insert		-				添加数据
 *
 *	@agentNode	[in]			添加的节点
 *	@state		[in]			节点状态
 *
 *	Note:						只在该模块的线程中使用，不用加锁
 *
 *	return
 *		!=0						添加成功
 *		==0						失败
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
 *	del		-				归还资源，并相应的初始化
 *
 *	@pNode			[in]		要删除的节点
 *
 *	return
 *					无
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
 *	destroy		-				释放资源,不用加锁(已经没人在用这个了)
 *
 *
 *	return
 *								无
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
			"从AGENTED队列中关闭套接字对[%d:%d]",
			pNode->m_stAgeNode.m_sCliSock, pNode->m_stAgeNode.m_sSrvSock);
		MSocket::close(pNode->m_stAgeNode.m_sCliSock);
		MSocket::close(pNode->m_stAgeNode.m_sSrvSock);

		pNode->m_stAgeNode.init();

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
 *	@cliSock	[in]			客户端套接字
 *	@srvSock	[in]			服务端套接字
 *	@dstIP		[in]			目标服务器IP
 *	@dstPort	[in]			目标服务器端口
 *	&applyType	[in]			类型(委托与非委托)
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
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
		"套接字对[%d:%d]完成连接(或者AGENT连接)",
		cliSock, srvSock);	
	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
}


/**
 *	del		-					取元素
 *
 *	@ageNode					取出的元素
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MAgentedMgr::del(AGENT_NODE	&ageNode)
{
	MLocalSection					locSec;							
	
	ageNode.init();
	
	locSec.Attch(&m_Lock);
	if(list_empty(&m_stUsed))/** 链表空*/
		return BUILD_ERROR(0, ELISTEMPTY);
	
	AGENT_LIST_NODE	*pNode = list_entry(m_stUsed.next, AGENT_LIST_NODE, m_listNode);
	memcpy(&ageNode, &(pNode->m_stAgeNode), sizeof(ageNode));

	/* 资源回归前重新初始化 */
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
	if(m_hHandHandle == -1)
		return BUILD_ERROR(_OSerrno(), ECRETHREAD);
	
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
	m_clAgentedMgr.destroy();
}

/**
 *	insert		-			添加元素
 *
 *	@cliSock	[in]			客户端套接字
 *	@srvSock	[in]			服务端套接字
 *	@dstIP		[in]			目标服务器IP
 *	@dstPort	[in]			目标服务器端口
 *	&applyType	[in]			类型(委托与非委托)
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MHand::insert(const SOCKET cliSock, const SOCKET srvSock, const char *dstIP, 
				  const unsigned short dstPort, const unsigned char applyType)
{
	return m_clAgentedMgr.insert(cliSock, srvSock, dstIP, dstPort, applyType);
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
		if(!local.isFull())/* 本地没满 */
		{
			if(pObj->m_clAgentedMgr.del(agentedNode) == 0)
			{
				if((rc = pObj->_sndHello(agentedNode)) == 0){	/* 成功发送HELLO包 */
					local.insert(agentedNode, HAND_SEND_HELLO_ED); /* 添加到本地，肯定是成功的，不用检验返回值 */
				}else{
					pObj->_doFailed(agentedNode, rc, HAND_CONN_ED);
				}
			}
		}

		if(!local.isEmpty())	/* 本地有数据 */
		{
			FD_ZERO(&fdRead);
			list_for_each(pList, &local.m_stUsed)/* 遍历链表添加select数据*/
			{
				pNode = list_entry(pList, HAND_LIST_NODE, m_listNode);	assert(pNode != NULL);
				/* 根据状态添加select */
				if(pNode->m_peerState == HAND_SEND_HELLO_ED || 
					pNode->m_peerState == HAND_SEND_OPEN_ED )
				{
					FD_SET(pNode->m_stAgentedNode.m_sSrvSock, &fdRead);
				}else{assert(0);}
			}

			rc = select(0, &fdRead, NULL, NULL, &tv);
			if(rc > 0)	/* 检测到套接字可读 */
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
							local.del(pNode);	/* 资源回归 */
						}else if(pNode->m_peerState == HAND_RECV_OPEN_ED) /*仅在_doRcv成功才会去判断是否完成握手*/
						{
							pObj->_doSucess(pNode);					/* 向下级添加 */
							local.del(pNode);	/* 资源回归 */
						}
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
 *	_doFailed		-			失败时的打印日志及套接字关闭
 *
 *	@ageNode		[in]		出错节点
 *	@errCode		[in]		出错原因
 *	@state			[in]		出错时状态
 *
 *	return
 *					无
 */
void MHand::_doFailed(AGENT_NODE &ageNode, int errCode, PEER_STATUS state)
{
	Global_Log.writeLog(LOG_TYPE_ERROR,
		"握手协议发生错误[%d(%d:%d)]状态为[%d], 关闭套接字对[%d:%d]",
		errCode, GET_SYS_ERR(errCode),GET_USER_ERR(errCode),
		state, ageNode.m_sCliSock, ageNode.m_sSrvSock);

	MSocket::close(ageNode.m_sCliSock);
	MSocket::close(ageNode.m_sSrvSock);
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
void MHand::_doSucess(HAND_LIST_NODE *peer)
{
	int						rc;

	assert(peer != NULL && peer->m_peerState == HAND_RECV_OPEN_ED);
	assert(peer->m_stAgentedNode.m_sCliSock != INVALID_SOCKET && peer->m_stAgentedNode.m_sSrvSock != INVALID_SOCKET);
	
	rc = m_clTranModule.insert(peer->m_stAgentedNode.m_sCliSock, peer->m_stAgentedNode.m_sSrvSock);
	if(rc != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
		"HAND线程向TRANS线程添加套接字对发生错误[%d]，关闭套接字对[%d:%d]",
		rc, peer->m_stAgentedNode.m_sCliSock, peer->m_stAgentedNode.m_sSrvSock);
		MSocket::close(peer->m_stAgentedNode.m_sCliSock);
		MSocket::close(peer->m_stAgentedNode.m_sSrvSock);
	}	
}


/**
 *	_rcvHello		-			收hello包并发open包
 *
 *	@peer			[in/out]	处理的节点
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
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

	rc = MSocket::rcvDataNonBlock(peer->m_stAgentedNode.m_sSrvSock, buffer, _rc); /** 测试点*/ 
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
 *	_sndOpen		-			发open包
 *
 *	@peer			[in/out]	处理的节点
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int MHand::_sndOpen(HAND_LIST_NODE *peer)
{
	assert(peer != NULL && peer->m_peerState == HAND_RECV_HELLO_ED);
	assert(peer->m_stAgentedNode.m_cDstIP[0] != 0 && 
		peer->m_stAgentedNode.m_usDstPort != 0);

	char					sndBuffer[1024];
	int						rc = 0;

	if(peer->m_stAgentedNode.m_cApplyType == 1) {/* 普通握手包 */
		rc = sprintf(sndBuffer, HTTP_UNIT_FMT_UP,
			peer->m_stAgentedNode.m_cDstIP, 
			peer->m_stAgentedNode.m_usDstPort,
			GCA_OPEN, 0, 0, 0);	/* 测试点: 都填的0 */
	}else if(peer->m_stAgentedNode.m_cApplyType == 2){	/* 委托握手包 */	
		rc = sprintf(sndBuffer, HTTP_UNIT_FMT_UP,
			peer->m_stAgentedNode.m_cDstIP, 
			peer->m_stAgentedNode.m_usDstPort,
			GCA_OPEN2, 0, 0, 0);/* 测试点: 都填的0 */
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
 *	_sndHello		-			发送Hello包
 *
 *	@agentedNode:		[in]	已完成代理的节点
 *
 *	return
 *		==0						成功
 *		!=0						失败
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
 *	_buildHello		-			组装hello包
 *
 *	@buffer:			[in]	缓冲区
 *	@bufsize			[in]	缓冲区大小
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int MHand::_buildHello(char *buffer, size_t bufsize)
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
 *	_rcvOpen		-			收open包，并回应客户端
 *
 *	@peer			[in/out]	收包对应的节点
 *
 *
 *	return
 *		==0						成功
 *		!=0						失败，上级需要关闭套接字对
 */
int	MHand::_rcvOpen(HAND_LIST_NODE *peer)
{
	assert(peer != NULL && peer->m_peerState == HAND_SEND_OPEN_ED);
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
	assert(rc < sizeof(rcvBuff));

	_rc = rc;	/* 缓存应收大小 */

	rc = MSocket::rcvDataNonBlock(peer->m_stAgentedNode.m_sSrvSock, rcvBuff, _rc);
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

	rc = MSocket::sendDataNonBlock(peer->m_stAgentedNode.m_sCliSock, commUnit,
		sizeof(tagQLGCProxy_Apply)+sizeof(tagQLGCProxy_CertifyRet));
	if(rc == 0)
		peer->m_peerState =  HAND_RECV_OPEN_ED;

	return rc;
}

/**
 *	_doRcv			-			收相应协议并处理相应协议
 *
 *	@peer			[in/out]	收数据的节点
 *	return
 *		==0						协议处理成功
 *		!=0						协议处理失败，该套接字对应该关闭
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
