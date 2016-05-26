#include <assert.h>
#include "MAgent.h"
#include "MGlobal.h"
#include "MSocket.h"
#include "error.h"
#include "MList.h"


/*
 *_MLOCALSOCK4_	-	构造函数，在线程中定义局部变量时的初始化
 */
_MLOCALAGENT_::_MLOCALAGENT_()
{
	INIT_LIST_HEAD(&m_stUsed);
	INIT_LIST_HEAD(&m_stFree);
	for(int i = 0; i < MAX_SOCK_COUNT; ++i)
	{
		m_sDataArr[i].m_eState = CONN_ED_INIT;
		m_sDataArr[i].m_stGcPro.init();
		list_add_tail(&(m_sDataArr[i].m_listNode), &m_stFree);
	}
}

/*
 *~_MLOCALSOCK4_	-	析构函数，在Hand线程退出后，完成套接字对的关闭
 */
_MLOCALAGENT_::~_MLOCALAGENT_()
{
	struct list_head		*pTmp;
	LOCAL_AGENT_NODE				*pNode;
	int						total = 0;/* 存储遗留的套接字对，方便查看问题 */
	
	list_for_each(pTmp, &m_stUsed)
	{
		++total;
		pNode = list_entry(pTmp, LOCAL_AGENT_NODE, m_listNode);
		Global_Log.writeLog(LOG_TYPE_INFO,
			"AGENT线程退出,套接字对状态为[%d],将关闭套接字对[%d:%d],",
			pNode->m_eState,
			pNode->m_stGcPro.m_sCliSock, 
			pNode->m_stGcPro.m_sSrvSock);
		MSocket::close(pNode->m_stGcPro.m_sCliSock);
		MSocket::close(pNode->m_stGcPro.m_sSrvSock);
	}
	if(total != 0){
		Global_Log.writeLog(LOG_TYPE_INFO,
			"AGENT线程退出后，共释放[%d]对套接字",
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
int _MLOCALAGENT_::isFull()
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
int	_MLOCALAGENT_::isEmpty()
{
	return list_empty(&m_stUsed);
}

/**
 *	insert		-				添加数据
 *
 *	@gcPro:		[in]			添加的数据对象
 *	@state		[in]			状态
 *
 *	Note:						只在该模块的线程中使用，不用加锁
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int	_MLOCALAGENT_::insert(GC_PRO &gcPro, const AGENT_STATUS state)
{	
	if(list_empty(&m_stFree))	/** list is empty*/
		return BUILD_ERROR(0, EHANDFULL);
	
	/** fill data*/
	LOCAL_AGENT_NODE	*pNode = list_entry(m_stFree.next, LOCAL_AGENT_NODE, m_listNode);
	pNode->m_eState = state;
	memcpy(&(pNode->m_stGcPro), &gcPro, sizeof(GC_PRO));

	return MList::_del2insert(&(pNode->m_listNode), &m_stUsed);
}

/**
 *	del		-					资源回归,删除节点
 *
 *	@pNode			[in]		需要回归的节点
 *
 *	return
 *								无
 */
void _MLOCALAGENT_::del(LOCAL_AGENT_NODE *pNode)
{
	pNode->m_eState = CONN_ED_INIT;
	pNode->m_stGcPro.init();
	list_del(&(pNode->m_listNode));
	list_add_tail(&(pNode->m_listNode), &m_stFree);
}


//////////////////////////////////////////////////////////////////////////

MAgent::MAgent()
{
	m_ulHandle = 0;
}

MAgent::~MAgent()
{

}

/**
 *	init		-				初始化
 *
 *	@pAgent:	[in/out]		MAgent模块的指针，在该对象中存储该指针，方便调用MAgent的函数
 *
 *	return
 *		==0						成功
 *		!=0						失败
 */
int	MAgent::init()
{
	int				rc;
	
	if((rc = m_clHandModl.init()) != 0)
		return rc;

	m_ulHandle =  _beginthreadex(NULL, 0, agentThread, this, 0, NULL);
	if(m_ulHandle == -1)
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
void MAgent::waitExit(int *err, size_t size)
{
	int					rc = 0;
	
	if(m_ulHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_ulHandle, WAIT_THREAD_TIMEOUT);
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待AGENT线程退出超时[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待AGENT线程退出出错[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}
		
		m_ulHandle = 0;
	}
	
	/* 释放本层资源 */
	destroy();
	/* 填充本层错误码 */
	err[0] = rc;
	assert(size > 1);
	
	/* 等待下层返回 */
	m_clHandModl.waitExit(++err, --size);
}

/**
 *	destroy		-				该层资源销毁
 *
 *	return
 *								无
 */
void MAgent::destroy()
{
	m_clConnedMgr.destroy();
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
int	MAgent::insert(GC_PRO &gcPro)
{
	int					rc;
	switch(gcPro.m_eType)
	{
	case DIRCONN:
		rc =  m_clHandModl.insert(gcPro.m_sCliSock, gcPro.m_sSrvSock, gcPro.m_cDstIP, gcPro.m_usDstPort, gcPro.m_ucApplyType);
		break;
	case SOCK4:
	case SOCK4A:
	case HTTP1_1:
	case SOCK5:
		rc = m_clConnedMgr.insert(gcPro);
		break;	
	default:
		assert(0);
		rc = BUILD_ERROR(0, EABORT);
		break;
	}
	
	return rc;
}
/**
 *	del		-					取元素
 *
 *	@gcPro		[in/out]		取出的元素
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MAgent::del(GC_PRO &gcPro)
{
	return m_clConnedMgr.del(gcPro);
}

/**
 *	agentThread		-			代理线程
 *
 *	@in			[in]			线程参数
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
unsigned int __stdcall MAgent::agentThread(void * in)
{
	MAgent				*pObj = (MAgent *)in;					/* MSOCK4对象 */
	MLOCALAGENT			local;									/* 线程local对象*/
	struct list_head	*pList, *pSafe;							/* 遍历时要用到的链表节点 */
	LOCAL_AGENT_NODE			*pNode;
	struct timeval		tv;
	fd_set				fdRead;
	GC_PRO				gcPro;
	int					rc;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);

	do{
		if(!local.isFull())/* 本地没满 */
		{
			if(pObj->m_clConnedMgr.del(gcPro) == 0)	/* mgr中取数据成功(mgr中有数据) */
			{
				assert(gcPro.m_sCliSock != INVALID_SOCKET && gcPro.m_sSrvSock != INVALID_SOCKET);
				if((rc = pObj->_sndAge(gcPro)) == 0)	/* 发代理包成功 */
					local.insert(gcPro, SEND_ED_HELLO);		/* 添加到本地，肯定是成功的，不用检验返回值 */
				else
					pObj->_doFailed(gcPro, rc, CONN_ED_INIT);
			}
		}
	
		if(!local.isEmpty())	/* 本地有数据 */
		{
			FD_ZERO(&fdRead);
			list_for_each(pList, &local.m_stUsed)/* 遍历链表添加select数据*/
			{
				pNode = list_entry(pList, LOCAL_AGENT_NODE, m_listNode); assert(pNode != NULL);
				if(pNode->m_eState == SEND_ED_HELLO || 
					pNode->m_eState == SEND_ED_AUTH ||
					pNode->m_eState == SEND_ED_AGENT)
				{
					FD_SET(pNode->m_stGcPro.m_sSrvSock, &fdRead);	
				}else {assert(0);}		
			}
			rc = select(0, &fdRead, NULL, NULL, &tv);
			if(rc > 0)/* 检测成功*/
			{
				list_for_each_safe(pList, pSafe, &local.m_stUsed)
				{
					pNode = list_entry(pList, LOCAL_AGENT_NODE, m_listNode); assert(pNode != NULL);
					if(FD_ISSET(pNode->m_stGcPro.m_sSrvSock, &fdRead)){
						if((rc = pObj->_rcvAge(pNode)) == 0 )	/* 成功收到返回包 */
						{
							if(pNode->m_eState == SUCC_ED)	/* 代理已完成 */
							{
								pObj->_doSucess(pNode);
								local.del(pNode); /* 资源回归 */
							}
						}
						else
						{
							pObj->_doFailed(pNode->m_stGcPro, rc, pNode->m_eState);
							local.del(pNode); /* 资源回归 */
						}	
					}
				}
			}else
			{
				if(rc < 0){/* 发生错误*/
					Global_Log.writeLog(LOG_TYPE_ERROR, "AGENT线程select出错[%d]\n", BUILD_ERROR(_OSerrno(), EABORT));
					assert(0);
				}
			}
		}else{Sleep(15);}	
	}while(Global_IsRun);

	/* 仅当用户取消才会返回 */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"AGENT线程退出");

	return 0;
}

/**
 *	_buildSOCK4		-			组装sock4协议请求包
 *
 *	@gcPro			[in]		协议包所对应的节点
 *	@sndBuf			[in]		组装数据的缓冲区
 *	@bufSize		[in]		组装数据缓冲区大小
 *	@sndSize		[in/out]	组装后数据的大小
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int MAgent::_buildSOCK4(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize)
{
	/*
		SOCK4 发送请求：
					+----+----+----+----+----+----+----+----+----+----+....+----+
		字段：		| VN | CD | DSTPORT |       DSTIP       |    USERID    |NULL|
					+----+----+----+----+----+----+----+----+----+----+....+----+
		字节数：	| 1  | 1  |    2    |         4         |   variable   | 1  |
					+----+----+----+----+----+----+----+----+----+----+....+----+

					VN			SOCKS协议版本号，这里必须填4
					CD			SOCKS的命令代码，对于建立连接的情况下，应该填1
					DSTPORT		目标服务器端口（2字节，网络字节序）
					DSTIP		目标服务器IP（4字节，网络字节序）
					USERID		用户ID（字符串，字节数不定）
					NULL		用户名称字符串的结束符，数值0
	*/
	assert(gcPro.m_eType == SOCK4);
	char			IP[32] = {0};
	addr4_req		*add4req	= (addr4_req*)sndBuf;
	int				rc;

	rc = MSocket::dnsToIP(gcPro.m_cGcSIP, sizeof(gcPro.m_cGcSIP), IP, sizeof(IP)-1);
	if(rc != 0)
			return rc;
	
	add4req->ver			= (char)0x04;				// 对于版本4这里是0x04
	add4req->cd				= (char)0x01;				// CONNECT
	add4req->dstport		= htons( gcPro.m_usGcSPort);	// 目标端口
	add4req->dstip			= inet_addr( gcPro.m_cGcSIP );	//	目标地址
	
	sndSize					= sizeof(addr4_req);
	*(sndBuf + sndSize)		= '\0';						// 没有用户id，最后必须填NULL
	sndSize					+= 1;

	if(sndSize > bufSize)
		return BUILD_ERROR(0, EBUFLESS);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"套接字对[%d:%d]使用代理方式[%d]通过代理服务器[%s:%d]向GcS[%s:%d]发起连接",
		gcPro.m_sCliSock, gcPro.m_sSrvSock,
		gcPro.m_eType,
		gcPro.m_cAgIP, gcPro.m_usAgPort,
		gcPro.m_cGcSIP, gcPro.m_usGcSPort);
	
	return 0;
}

/**
 *	_buildSOCK4A		-		组装sock4A协议请求包
 *
 *	@gcPro			[in]		协议包所对应的节点
 *	@sndBuf			[in]		组装数据的缓冲区
 *	@bufSize		[in]		组装数据缓冲区大小
 *	@sndSize		[in/out]	组装后数据的大小
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int MAgent::_buildSOCK4A(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize)
{
	/*

		SOCK4A 发送请求：
				  +——+——+——+——+——+——+——+——+——+——+....+——+——+——+....+——+
				  | VN | CD | DSTPORT | DSTIP 0.0.0.x     | USERID       |NULL| HOSTNAME     |NULL|
				  +——+——+——+——+——+——+——+——+——+——+....+——+——+——+....+——+
				   1    1      2              4           variable       1    variable       1
	*/
	assert(gcPro.m_eType == SOCK4A);
	unsigned short	connPort= htons( gcPro.m_usGcSPort);

	sndBuf[0]					= (char)0x04;				// 对于版本4这里是0x04
	sndBuf[1]					= (char)0x01;				// CONNECT
	memcpy(&sndBuf[2],&connPort,2);							//目标服务器端口
	//Set the IP to 0.0.0.x (x is nonzero)
	sndBuf[4]					= 0;
	sndBuf[5]					= 0;
	sndBuf[6]					= 0;
	sndBuf[7]					= 1;
	sndBuf[8]					= '\0';
	//Add host as URL
	sndSize = 9;	//已经填了9个字节了
	strncpy(&sndBuf[9], gcPro.m_cGcSIP, sizeof(gcPro.m_cGcSIP));//目标服务器IP
	sndSize += strlen(gcPro.m_cGcSIP);
	sndBuf[sndSize] = '\0';
	sndSize += 1;

	if(sndSize > bufSize)
		return BUILD_ERROR(0, EBUFLESS);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"套接字对[%d:%d]使用代理方式[%d]通过代理服务器[%s:%d]向GcS[%s:%d]发起连接",
		gcPro.m_sCliSock, gcPro.m_sSrvSock,
		gcPro.m_eType,
		gcPro.m_cAgIP, gcPro.m_usAgPort,
		gcPro.m_cGcSIP, gcPro.m_usGcSPort);

	return 0;
}

/**
 * _base64			- base64加密
 *
 *	@in_buf			[in]	源串
 *	@out_buf		[out]	加密串
 *	@size			[int]	源串长度
 *
 *	return:
 *			加密串的长度
 */
int MAgent::_base64( IN const char * in_buf, OUT char * out_buf, IN int size )
{
	char	base64_encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int		buflen = 0; 

	while( size>0 )
	{
		*out_buf++ = base64_encoding[ (in_buf[0] >> 2 ) & 0x3f];
		if( size>2 )
		{
			*out_buf++ = base64_encoding[((in_buf[0] & 3) << 4) | (in_buf[1] >> 4)];
			*out_buf++ = base64_encoding[((in_buf[1] & 0xF) << 2) | (in_buf[2] >> 6)];
			*out_buf++ = base64_encoding[in_buf[2] & 0x3F];
		}
		else
		{
			switch( size )
			{
			case 1:
				*out_buf++ = base64_encoding[(in_buf[0] & 3) << 4 ];
				*out_buf++ = '=';
				*out_buf++ = '=';
				break;
			case 2: 
				*out_buf++ = base64_encoding[((in_buf[0] & 3) << 4) | (in_buf[1] >> 4)]; 
				*out_buf++ = base64_encoding[((in_buf[1] & 0x0F) << 2) | (in_buf[2] >> 6)]; 
				*out_buf++ = '='; 
				break; 
			} 
		} 
		in_buf +=3; 
		size -=3; 
		buflen +=4; 
	} 
	*out_buf = 0; 
	return buflen;
}


/**
 *	_buildHttp_1_1		-		组装HTTP1.1协议请求包
 *
 *	@gcPro			[in]		协议包所对应的节点
 *	@sndBuf			[in]		组装数据的缓冲区
 *	@bufSize		[in]		组装数据缓冲区大小
 *	@sndSize		[in/out]	组装后数据的大小
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int MAgent::_buildHttp_1_1(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize)
{
	assert(gcPro.m_eType == HTTP1_1);

	char				auth[512], out_auth[512];
	int					size;
	
	if(gcPro.m_bNeedAuth)	/* 需要认证 */
	{
		if(gcPro.m_cUrName[0] == '\0' || gcPro.m_cPassWd[0] == '\0'){
			sndErrToCli(E_ERR_NULLCHAR, gcPro.m_sCliSock);	/* 给用户出错信息 */
			return BUILD_ERROR(0, E_ERR_NULLCHAR);
		}

		size			= _snprintf( auth, sizeof(auth), "%s:%s", gcPro.m_cUrName, gcPro.m_cPassWd );
		size			= _base64( auth, out_auth, size );
		out_auth[size]	= '\0';
		
		sndSize = _snprintf( sndBuf, bufSize,	
			"CONNECT %s:%d HTTP/1.1\r\n"
			"Proxy-Connection: Keep-Alive\r\n"
			"Content-Length: 0\r\n"
			"Host: %s:%d\r\n"
			"Authorization: Basic %s\r\n"
			"Proxy-Authorization: Basic %s\r\n"
			"\r\n",
		gcPro.m_cGcSIP, gcPro.m_usGcSPort, 
		gcPro.m_cGcSIP, gcPro.m_usGcSPort, 
		out_auth, out_auth ); /* Test point */
	}else		/* 不需要认证 */
	{
		sndSize = _snprintf( sndBuf, bufSize,
			"CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\n\r\n",
			gcPro.m_cGcSIP, gcPro.m_usGcSPort, 
			gcPro.m_cGcSIP, gcPro.m_usGcSPort );
	}

	if(sndSize > bufSize)
			return BUILD_ERROR(0, EBUFLESS);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"套接字对[%d:%d]使用代理方式[%d],通过代理服务器[%s:%d]向GcS[%s:%d]发起连接",
		gcPro.m_sCliSock, gcPro.m_sSrvSock,
		gcPro.m_eType,
		gcPro.m_cAgIP, gcPro.m_usAgPort,
		gcPro.m_cGcSIP, gcPro.m_usGcSPort);

	return 0;
}

/**
 *	_sndSock5Auth		-		向SOCK5发送认证请求
 *
 *	@gcPro			[in]		发送认证请求所在节点
 *	@sndBuf			[in]		组装数据的缓冲区
 *	@bufSize		[in]		组装数据缓冲区大小
 *	@sndSize		[in/out]	组装后数据的大小
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MAgent::_buildSock5Auth(const GC_PRO &gcPro, char *sndBuf, size_t bufSize, size_t &sndSize)
{
	/*
		给代理发送认证协商请求：
					+----+----------+----------+
		字段		|VER | NMETHODS | METHODS  |
					+----+----------+----------+
		字节数		| 1  |    1     | 1 to 255 |
					+----+----------+----------+
					对于SOCKS5，VER字段为0x05，版本4对应0x04。NMETHODS字段指定METHODS域的字节
				数。不知NMETHODS可以为0否，看上图所示，可取值[1,255]。METHODS字段有多少字节(假
				设不重复)，就意味着SOCKS Client支持多少种认证机制。
	 */

	assert(gcPro.m_eType == SOCK5);

	sndBuf[0]	= (char)0x05;		// VER字段, SOCKS5为0x05，SOCKS4为0x04
	sndBuf[1]	= (char)0x02;		// NMETHODS字段，指明了两种认证方式，下面两个字节buf[2]和buf[3]是这两种认证方式的代号
	sndBuf[2]	= (char)0x00;		// 0x00 表示无需认证
	sndBuf[3]	= (char)0x02;		// 0x02 表示用户密码方式认证
	sndSize	= 4;
	
	return 0;
}

/**
 *	_sendAge		-			代理组装第一个包并发送代理包
 *
 *	@gcPro			[in]		发送包所在节点	
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int	MAgent::_sndAge(GC_PRO &gcPro)
{
	char			sndBuffer[1024] = {0};
	size_t			sndSize = 0;
	int				rc;

	switch ( gcPro.m_eType )
	{
	case SOCK4:
		rc = _buildSOCK4(gcPro, sndBuffer, sizeof(sndBuffer), sndSize);
		break;
	case SOCK4A:
		rc = _buildSOCK4A(gcPro, sndBuffer, sizeof(sndBuffer), sndSize);
		break;
	case HTTP1_1:
		rc = _buildHttp_1_1(gcPro, sndBuffer, sizeof(sndBuffer), sndSize);
		break;
	case SOCK5:
		rc = _buildSock5Auth(gcPro, sndBuffer, sizeof(sndBuffer), sndSize);
		break;
	default:
		assert(0);
		rc = BUILD_ERROR(0, EABORT);
		break;
	}
	
	if(rc != 0)
		return rc;

	/* 发代理包 */
	return MSocket::sendDataNonBlock(gcPro.m_sSrvSock, sndBuffer, sndSize);
}


/**
 *	_rcvAge		-				收SOCK4或SOCKA代理包
 *
 *	@node			[in]		收包所对应节点
 *
 *	return
 *		== 0					成功
 *		!= 0					出错
 */
int MAgent::_rcvSOCK4Age(LOCAL_AGENT_NODE *node)
{
	addr4_res			rcvData;
	int					rcvAgeSize = sizeof(addr4_res);
	int					rc;
	
	assert(node->m_eState == SEND_ED_HELLO);
	memset(&rcvData, 0, sizeof(rcvData));
	rc = MSocket::rcvDataNonBlock(node->m_stGcPro.m_sSrvSock,
		(char*)&rcvData, rcvAgeSize);
	if(rc != 0)
		return rc;
	
	if( (char)90 == rcvData.cd ){ /* 认证成功 */
		node->m_eState = SUCC_ED;
		return 0;
	}else{
		if( (char)91 == rcvData.cd ){
			rc = ESOCK4_91;
		}else if( (char)92 == rcvData.cd ){
			rc = ESOCK4_92;
		}else if( (char)93 == rcvData.cd ){
			rc = ESOCK4_93;
		}else{
			rc = ESOCK4_UNKNOWN;
		}
		
		sndErrToCli(rc, node->m_stGcPro.m_sCliSock); /* 向客户端返回认证出错的错误码 */
		return BUILD_ERROR(0, rc);
	}	
}

/**
 *	_sndSOCK5Auth	-			发送SOCK5认证信息
 *
 *	@node			[in]		发包所对应节点
 *
 *	return
 *		== 0					成功
 *		!= 0					出错
 */
int MAgent::_sndSOCK5Auth(LOCAL_AGENT_NODE *node)
{
	/**
					+----+------+----------+------+----------+
		字段		|VER | ULEN |   UNAME  | PLEN |  PASSWD  |
					+----+------+----------+------+----------+
		字节数		| 1  |  1   | 1 to 255 |  1   | 1 to 255 |
					+----+------+----------+------+----------+

  					VER				X'01'
					ULEN			用户名长度
					UNAME			用户名（不带串结束符）
					PLEN			密码长度
					PASSWD			密码（不带串结束符）
	 */
	assert(node->m_eState == SEND_ED_HELLO);
	char				buf[1024];
	unsigned char		temp;
	int					size, res;
	int					sockfd = node->m_stGcPro.m_sSrvSock;
	
	if(node->m_stGcPro.m_bNeedAuth != true)
	{
		sndErrToCli(ERR_USER_BAUTH, node->m_stGcPro.m_sCliSock);//回馈用户错误码
		return BUILD_ERROR(0, ERR_USER_BAUTH);
	}
	
	if ( node->m_stGcPro.m_cUrName[0] == 0 || 
		node->m_stGcPro.m_cPassWd[0] == 0 )
	{
		return BUILD_ERROR(0, E_ERR_NULLCHAR);
	}

	size		= 0;
	buf[size]	= 1;				// VER
	size++;
	
	temp = (unsigned char)strlen( node->m_stGcPro.m_cUrName );
	if ( 0 == temp )	
	{
		return BUILD_ERROR(0, E_ERR_NULLCHAR);
	}
	
	buf[size] = (char)temp;						// ULEN
	size++;
	strncpy( buf + size, node->m_stGcPro.m_cUrName, temp );			// UNAME
	size += temp;
	
	temp = (unsigned char)strlen( node->m_stGcPro.m_cPassWd );
	if ( 0 == temp )
	{
		return BUILD_ERROR(0, E_ERR_NULLCHAR);
	}
	
	buf[size] = (char)temp;						// PLEN
	size++;
	strncpy( buf + size, node->m_stGcPro.m_cPassWd, temp );		// PASSWD
	size += temp;
	
	res = MSocket::sendDataNonBlock(sockfd, buf, size);
	if(res != 0)		return res;

	node->m_eState = SEND_ED_AUTH;

	return 0;
}
/**
 *	_rcvSOCK5Hello2sndAuth	-	收SOCK5 hello包,如果需要认证再发送认证请求
 *
 *	@node			[in]		收包所对应节点
 *
 *	return
 *		== 0					成功
 *		!= 0					出错
 */
int	MAgent::_rcvSOCK5Hello2sndAuth(LOCAL_AGENT_NODE *node)
{
	/*
		接收代理的响应：
					+----+--------+
		字段		|VER | METHOD |
					+----+--------+
		字节数		| 1  |   1    |
					+----+--------+

					目前可用METHOD值有:
						X'00'       		NO AUTHENTICATION REQUIRED(无需认证)
						X'01'        		GSSAPI
						X'02'        		USERNAME/PASSWORD(用户名/口令认证机制)
						X'03'-X'7F'			IANA ASSIGNED
						X'80'-X'FE'			RESERVED FOR PRIVATE METHODS(私有认证机制)
						X'FF'        		NO ACCEPTABLE METHODS(完全不兼容)
	*/
	assert(node->m_eState == SEND_ED_HELLO);
	
	int						rc;
	char					buf[2] = {0};
	char					auth;

	rc = MSocket::rcvDataNonBlock(node->m_stGcPro.m_sSrvSock, buf, 2);
	if(rc != 0)	return rc;

	auth = buf[1];

	switch(auth)
	{
	case NOTNEED:
		rc = _sndSOCK5Agent(node);
		break;
	case USER_PWD:
		rc = _sndSOCK5Auth(node);
		break;
	default:
		rc = BUILD_ERROR(0, ERR_UNKNOW_AUTH);
		break;
	}

	return rc;
}

/**
 *	_sndSOCK5Agent	-			SOCK5发送代理信息
 *
 *	@node			[in]		收包所对应节点
 *
 *	return
 *		== 0					成功
 *		!= 0					出错
 */
int MAgent::_sndSOCK5Agent(LOCAL_AGENT_NODE *node)
{
	/*
		发送请求：
					+----+-----+-------+------+----------+----------+
		字段		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
					+----+-----+-------+------+----------+----------+
		字节数		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

					VER			对于版本5这里是0x05
					CMD			可取如下值:
								X'01'	CONNECT
								X'02'	BIND
								X'03'	UDP ASSOCIATE
					RSV			保留字段，必须为0x00
					ATYP		用于指明DST.ADDR域的类型，可取如下值:
	        					X'01'	IPv4地址
	    						X'03'	FQDN(全称域名)
	    						X'04'	IPv6地址
					DST.ADDR	CMD相关的地址信息，不要为DST所迷惑
								如果是IPv4地址，这里是big-endian序的4字节数据
								如果是FQDN，比如"www.nsfocus.net"，这里将是:
								0F 77 77 77 2E 6E 73 66 6F 63 75 73 2E 6E 65 74
								注意，没有结尾的NUL字符，非ASCIZ串，第一字节是长度域
								如果是IPv6地址，这里是16字节数据。
					DST.PORT	CMD相关的端口信息，big-endian序的2字节数据
	*/

	assert(node->m_eState == SEND_ED_HELLO || node->m_eState == SEND_ED_AUTH);
	char			buf[256];
	int				res, size, len;
	addr_req*		addreq	= (addr_req*)buf;
	addr_res*		addres	= (addr_res*)buf;

	addreq->ver		= (char)0x05;		// 对于版本5这里是0x05
	addreq->cmd		= (char)0x01;		// CONNECT
	addreq->rsv		= (char)0x00;		// 保留，必须为0
	addreq->atyp	= (char)0x03;		// 用于指明DST.ADDR域的类型，可取如下值
										// X'01'	IPv4地址
										// X'03'	FQDN(全称域名)
										// X'04'	IPv6地址

	size			= sizeof(addr_req);
	len				= strlen(node->m_stGcPro.m_cGcSIP);
	buf[size]		= (char)len;
	size++;
	memcpy( buf + size, node->m_stGcPro.m_cGcSIP, len );
	size			+= len;
	*(unsigned short*)(buf + size) = htons( node->m_stGcPro.m_usGcSPort );
	size			+= sizeof(unsigned short);

	Global_Log.writeLog(LOG_TYPE_INFO,
		"套接字对[%d:%d]使用代理方式[%d],通过代理服务器[%s:%d]向GcS[%s:%d]发起连接",
		node->m_stGcPro.m_sCliSock, node->m_stGcPro.m_sSrvSock,
		node->m_stGcPro.m_eType,
		node->m_stGcPro.m_cAgIP, node->m_stGcPro.m_usAgPort,
		node->m_stGcPro.m_cGcSIP, node->m_stGcPro.m_usGcSPort);
	
	res = MSocket::sendDataNonBlock(node->m_stGcPro.m_sSrvSock, buf, size);
	if(res != 0)	return res;

	node->m_eState = SEND_ED_AGENT;

	return 0;
}

/**
 *	_rcvAuth2sndAgent	-		收SOCK5认证信息,并发送代理信息
 *
 *	@node			[in]		收包所对应节点
 *
 *	return
 *		== 0					成功
 *		!= 0					出错
 */
int	MAgent::_rcvSOCK5Auth2sndAgent(LOCAL_AGENT_NODE *node)
{
	/**
					+----+--------+
		字段		|VER | STATUS |
					+----+--------+
		字节数		| 1  |   1    |
					+----+--------+

					STATUS			X'00'为成功,否则断开连接
	 */
	assert(node->m_eState == SEND_ED_AUTH);

	char				buf[2];
	int					rc;
	
	memset(buf, 0, sizeof(buf));
	rc = MSocket::rcvDataNonBlock(node->m_stGcPro.m_sSrvSock, buf, 2);
	if(rc != 0)			return rc;

	/* 校验是否认证成功 */
	if ( (char)0x00 != buf[1] )
	{
		return BUILD_ERROR(0, E_ERR_SOCK5);
	}

	return _sndSOCK5Agent(node);
}

/**
 *	_rcvSOCK5		-			收SOCK5代理包
 *
 *	@node			[in]		收包所对应节点
 *
 *	return
 *		== 0					成功
 *		!= 0					出错
 */
int MAgent::_rcvSOCK5(LOCAL_AGENT_NODE *node)
{
	assert(node != NULL);
	assert(node->m_eState != SUCC_ED);
	assert(node->m_stGcPro.m_eType == SOCK5);

	int							rc;

	switch(node->m_eState)
	{
	case SEND_ED_HELLO:
		rc = _rcvSOCK5Hello2sndAuth(node);
		break;
	case SEND_ED_AUTH:
		rc = _rcvSOCK5Auth2sndAgent(node);
		break;
	case SEND_ED_AGENT:
		rc = _rcvSOCK5Agent(node);
		break;
	default:
		rc = BUILD_ERROR(0, EABORT);
	}

	return rc;
}


/**
 *	_rcvSOCK5Agent	-			收SOCK5代理信息
 *
 *	@node			[in]		收包所对应节点
 *
 *	return
 *		== 0					成功
 *		!= 0					出错
 */
int	MAgent::_rcvSOCK5Agent(LOCAL_AGENT_NODE *node)
{
	/*
		接收响应：
					+----+-----+-------+------+----------+----------+
		字段		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
					+----+-----+-------+------+----------+----------+
		字节数		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

					VER         对于版本5这里是0x05
					REP         可取如下值:
								0x00        成功
								0x01        一般性失败
        						0x02        规则不允许转发
								0x03        网络不可达
								0x04        主机不可达
								0x05        连接拒绝
								0x06        TTL超时
								0x07        不支持请求包中的CMD
        						0x08        不支持请求包中的ATYP
								0x09-0xFF   unassigned
					RSV         保留字段，必须为0x00
					ATYP        用于指明BND.ADDR域的类型
					BND.ADDR    CMD相关的地址信息，不要为BND所迷惑
					BND.PORT    CMD相关的端口信息，big-endian序的2字节数据
	 */

	assert(node->m_eState == SEND_ED_AGENT);
	char			buf[256];
	int				res, error, size;
	addr_res*		addres	= (addr_res*)buf;
	int				sockfd = node->m_stGcPro.m_sSrvSock;

	res = MSocket::rcvDataNonBlock(sockfd, (char*)addres, sizeof(addr_res));
	if(res != 0)		return res;

	res	= 0;
	if ( (char)0x00 == addres->rep )
	{
		switch( addres->atyp )
		{
		case 0x01:			// IPv4地址 + port
			error	= MSocket::rcvDataNonBlock( sockfd, (char*)(&addres + 1), 6);
			if ( 0 != error )	return error;
			break;
		case 0x03:
			size	= sizeof(addr_res);
			error	= MSocket::rcvDataNonBlock( sockfd, buf + size, 1);
			if ( 0 != error )	return error;
			
			size++;
			error	= MSocket::rcvDataNonBlock( sockfd, buf + size, buf[size-1]);
			if ( 0 != error )	return error;
			
			size	+= buf[size-1];
			error	= MSocket::rcvDataNonBlock( sockfd, buf + size, 2);
			if ( 0 != error )	return error;
			break;
		case 0x04:
			error	= MSocket::rcvDataNonBlock( sockfd, (char*)(&addres + 1), 10 );
			if ( 0 != error )	return error;
			break;
		default:
			res	= BUILD_ERROR(0, ERR_UNKNOW_ADDRTYPE);
			break;
		}
	}
	else
		res = BUILD_ERROR( 0, ERR_SERVICE_REJ );

	if(res == 0)
		node->m_eState = SUCC_ED;

	return res;
}
/**
 * _check_http_response			- 检查http代理服务器的应答信息
 *
 *	@buf			[in]应答信息
 *	@size			[in]应答信息大小
 *
 *	return:
 *			==	0	验证成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int MAgent::_get_http_response_code(  char* buf,  int size )
{
	int			i, res;
	int			start = 0;

	// trim掉协议前面的LWS
	for ( i = 0; i < size; i++ )
	{
		if ( (' ' != buf[i]) && ('\t' != buf[i]) &&
				('\r' != buf[i]) && ('\n' != buf[i]) )
		{
			start = i;
			break;
		}
	}

	if ( i == size )
		return BUILD_ERROR(0, E_ERR_HTTP);

	res = strncmp( buf + start, "HTTP/", strlen("HTTP/") );
	if ( 0 != res )
		return BUILD_ERROR(0, E_ERR_HTTP);

	for( i = start + strlen("HTTP/"); i < size - 3; i++ )
	{
		if ( ' ' == buf[i] )
		{
			return atol(buf+i+1);
		}
	}

	return 0;
}

/**
 * _grab_content_lenth			- 抓取响应信息中的Content-Length标签
 *
 *	@buf			[in]http响应协议buf
 *
 *	return:
 *			==	0	没有Content-Length标签
 *			>	0	Content-Length标签的值
 *			<	0	出错了，标签的值不正确
 */
int MAgent::_grab_content_lenth(  char* buf )
{
	int			i, j;
	char		len[16] = {0}, *pLen;

	pLen = strstr( buf, "Content-Length" );
	if ( NULL == pLen )		return 0;

	for ( i = strlen("Content-Length"); '\r' != pLen[i]; i++ )
	{
		if ( ':' == pLen[i] )
			break;
	}

	if ( '\r' == pLen[i] )
		return 0;

	for ( i++; '\r' != pLen[i]; i++ )
	{
		if ( (' ' != pLen[i]) && ('\t' != pLen[i]) &&
			('\r' != pLen[i]) && ('\n' != pLen[i]) )
			break;
	}
	
	if ( '\r' == pLen[i] || '\n' == pLen[i] )
		return 0;

	for ( i, j = 0; (('\r' != pLen[i]) && ('\n' != pLen[i])) && (15 > j); i++, j++ )
	{
		if ( !('0' > pLen[i] || '9' < pLen[i]) )
			len[j] = pLen[i];
		else if ( (' ' != pLen[i]) && ('\t' != pLen[i]) )
			return BUILD_ERROR(0, E_ERR_HTTP);
	}

	return atol(len);
}


/**
 *	_rcvHTTPAge		-			收HTTP代理包
 *
 *	@node			[in]		收包所对应节点
 *
 *	return
 *		== 0					成功
 *		!= 0					认证的错误码
 */
int MAgent::_rcvHTTPAge(LOCAL_AGENT_NODE *node)
{
	char				buf[1024] = {0};
	int					res, flag = 1, CRLF2 = 0x0A0D0A0D;		/* /r/n/r/n */
	int					sockfd = node->m_stGcPro.m_sSrvSock;
	int					rsize = 0;		/*收到的http协议数据大小*/

	assert(node->m_eState == SEND_ED_HELLO);

	/* step1:收http协议数据*/
	res = MSocket::rcvDataNonBlock( sockfd, buf + rsize, 4 );
	if ( 0 != res )		return res;

	rsize += 4;
	while( (*(int*)(buf + rsize - 4) != CRLF2) && (rsize < sizeof(buf)) )
	{
		res = MSocket::rcvDataNonBlock( sockfd, buf + rsize, 1 );
		if ( 0 != res )		return res;
		++rsize;
	}

	/* step2: check*/
	res = _get_http_response_code(buf, rsize);
	if(200 != res)
	{
		Global_Log.writeLog(LOG_TYPE_WARN,
			"http代理返回的错误码为[%d]", res);

		sndErrToCli(res, node->m_stGcPro.m_sCliSock);

		return BUILD_ERROR(0, ERR_HTTP_RESPONSE);
	}

	/* step3:得到数据体大小并收数据*/
	rsize = _grab_content_lenth( buf );
	if ( 0 < rsize )
	{
		res = MSocket::rcvDataNonBlock( sockfd, buf, min(sizeof(buf), rsize) );
		if(0 != res)		return res;
	}
	else if ( 0 > rsize )
	{
		return	BUILD_ERROR(0, E_ERR_HTTP);
	}

	/* 认证成功 */
	node->m_eState = SUCC_ED;
	
	return	0;
}
/**
 *	_rcvAge		-				SOCK4代理收代理包
 *
 *	@gcNode			[in]		收包对象
 *
 *	return
 *		== 0					成功
 *		!= 0					失败
 */
int MAgent::_rcvAge(LOCAL_AGENT_NODE *node)
{
	assert(node != NULL);

	int				rc;
	switch(node->m_stGcPro.m_eType)
	{
	case SOCK4:
	case SOCK4A:
		rc = _rcvSOCK4Age(node);
		break;
	case HTTP1_1:
		rc = _rcvHTTPAge(node);
		break;
	case SOCK5:
		rc = _rcvSOCK5(node);
		break;
	default:
		assert(0);
		rc = BUILD_ERROR(0, EABORT);
		break;	
	}

	return rc;
}

/**
 *	_sendAge		-			出错时的打印日志及关闭套接字
 *
 *	@gcPro			[in]		出错所对应的对象
 *	@errCode		[in]		出错的错误码
 *	@state			[in]		出错时的状态
 *
 *	return
 *								无
 */
void MAgent::_doFailed(const GC_PRO &gcPro, const int errCode, const AGENT_STATUS state)
{
	Global_Log.writeLog(LOG_TYPE_ERROR,
		"以代理方式[%d]处理套接字对[%d:%d]出错[%d(%d:%d)],此时状态为[%d],关闭该套接字对",
		gcPro.m_eType, gcPro.m_sCliSock, gcPro.m_sSrvSock, errCode,
		GET_SYS_ERR(errCode), GET_USER_ERR(errCode), state);

	MSocket::close(gcPro.m_sCliSock);
	MSocket::close(gcPro.m_sSrvSock);
}


/**
 *	_doSucess		-			成功往下层添加数据
 *
 *	@node			[in]		指向成功的节点
 *
 *	return
 *								无
 */
void MAgent::_doSucess(const LOCAL_AGENT_NODE *node)
{
	assert(node->m_eState == SUCC_ED);
	int					rc;

	rc = m_clHandModl.insert(
		node->m_stGcPro.m_sCliSock,
		node->m_stGcPro.m_sSrvSock,
		node->m_stGcPro.m_cDstIP,
		node->m_stGcPro.m_usDstPort,
		node->m_stGcPro.m_ucApplyType);

	if(rc != 0)
		_doFailed(node->m_stGcPro, rc, node->m_eState);
}

