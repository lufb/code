#include <assert.h>
#include "MListen.h"
#include "MGlobal.h"
#include "MSocket.h"



MListen::MListen()
{
	m_LisHandle = 0;
	m_acceptCount = 0;
}

MListen::~MListen()
{

}

/**
 *	waitUserSetGc		-	等待用户设置GcS
 *
 *
 *	return
 *							无
 */
void MListen::waitUserSetGc()
{
	/* 等待用户设置GcSIP与GcSPort */
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
		"用户已经设了GcSIP与GcSPort,开始进行连接上级服务器");
	else
		Global_Log.writeLog(LOG_TYPE_WARN,
		"用户还没设GcSIP与GcSPort,用户就执行了退出");
}
	
/**
 *	listenThread		-	监听线程处理函数
 *
 *	@in:		[in]		线程参数
 *
 *	return
 *		==0					正确
 *		!=0					处理出错
 */
unsigned int __stdcall	MListen::listenThread(void * in)
{
	MListen					*pObj = (MListen*)in;
	SOCKET					lisSock = pObj->m_LisSock;
	fd_set					fdread;
	struct timeval			tv;
	int						rc = 0;

	INIT_TIMEVAL(tv, SELECT_TIME_OUT);

	listen(lisSock, 5);	

	pObj->waitUserSetGc();

	do{
		FD_ZERO(&fdread);
		FD_SET(lisSock, &fdread);

		rc = select(0, &fdread, NULL, NULL, &tv);
		switch (rc)
		{
		case SOCKET_ERROR:		/* 错误发生 */
			rc = BUILD_ERROR(_OSerrno(), EABORT);
			break;
		case 0:					/* select 超时了 */
			rc = 0;
			break;
		default:				/* 检测到可读套接字 */
			rc = pObj->acceptCli(fdread);
			break;
		}
		if(rc != 0)
			Global_Log.writeLog(LOG_TYPE_ERROR,	"监听线程发生错误[%d]", rc);
	}while(Global_IsRun);

	/* 仅在用户取消时才退出 */
	Global_Log.writeLog(LOG_TYPE_INFO,
		"LISTNE线程退出");

	return 0;
}

/**
 *	init		-			监听模块初始化
 *
 *	@port:		[in]		监听的端口
 *	@lisSock	[in]		创建的监听套接字
 *
 *	return
 *		==0					正确
 *		!=0					处理
 */
int	MListen::init(unsigned short port, SOCKET lisSock)
{
	int						rc;

	assert(port != 0 && lisSock != INVALID_SOCKET);

	rc = m_clRcvOpenMode.init();
	if(rc != 0)
		return rc;

	m_usLisPort = port;
	m_LisSock = lisSock;

	m_LisHandle = _beginthreadex(NULL, 0, listenThread, this, 0, NULL);
	if(m_LisHandle == -1)
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
void MListen::destroy()
{
	Global_Log.writeLog(LOG_TYPE_INFO,
		"通过监听套接字端口[%d]共accept了[%d]个套接字,关闭监听套接字[%d]", 
		m_LisSock, m_acceptCount, m_LisSock);
	m_acceptCount = 0;
	MSocket::close(m_LisSock);
	m_LisSock = INVALID_SOCKET;
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
void MListen::waitExit(int *err, size_t size)
{
	int					rc = 0;

	if(m_LisHandle != 0)
	{
		rc = ::waitThreadExit(1, (HANDLE*)&m_LisHandle, WAIT_THREAD_TIMEOUT);
		if(rc == WAIT_TIMEOUT){
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待LISTEN线程退出超时[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}else if(rc == WAIT_FAILED)
		{
			Global_Log.writeLog(LOG_TYPE_ERROR, 
				"等待LISTEN线程退出出错[%d]", 
				BUILD_ERROR(_OSerrno(), EWAITERROR));
			rc = BUILD_ERROR(_OSerrno(), EWAITERROR);
		}

		m_LisHandle = 0;
	}	

	/* 释放本层资源 */
	destroy();
	err[0] = rc;
	assert(size > 1);
	
	m_clRcvOpenMode.waitExit(++err, --size);
}

int	MListen::acceptCli(fd_set &fdread)
{
	SOCKET					cliSock;
	struct sockaddr_in		client;
	int						iAddrSize = sizeof(client);
	int						rc = 0;
	
	if(FD_ISSET(m_LisSock, &fdread))
	{
		cliSock = accept(m_LisSock, (struct sockaddr *)&client,
								&iAddrSize);
		if(cliSock == INVALID_SOCKET)
			return BUILD_ERROR(_OSerrno(), EABORT);

		++m_acceptCount;
		Global_Log.writeLog(LOG_TYPE_INFO,
					"通过监听端口[%d] 从[%s:%d] 收到socket[%d]",
					m_usLisPort, inet_ntoa(client.sin_addr),
					ntohs(client.sin_port), cliSock);

		/* 设置客户端套接字属性*/
		MSocket::setSock(cliSock, SOCK_RCV2SND_BUFF_SIZE);

		/* 添加到下级 */
		if((rc = m_clRcvOpenMode.insert(cliSock)) != 0){
			Global_Log.writeLog(LOG_TYPE_WARN,
				"LISTEN往RCVOPEN添加套接字[%d]出错，关闭套接字[%d]",
				cliSock, cliSock);
			MSocket::close(cliSock);
		}
	}else
	{
		assert(0);
		rc = BUILD_ERROR(_OSerrno(), EABORT);
	}

	return rc;
}




