/**
 *	main.c
 *
 *	Copyright (C) 2013 卢富波 <1164830775@qq.com>
 *
 *	修改历史:
 *
 *	2013--11-08 - 首次创建
 *
 *  程序描述:     
 *			学习使用select(数据转发)
 */

#include <stdio.h>
#include <winsock2.h>
#include <process.h>
#include <assert.h>
#include "main.h"
#include "error.h"
#include "http.h"

bool Global_Run = true;


//unsigned int WINAPI dealClientThread(LPVOID lpParameter);
int connDst(char *dstIP, unsigned short dstPort, SOCKET	&srvSock);

/************************************************************************/
/* usage: ./a.out bindPort DstIP DstPort                                */
/************************************************************************/
// int main(int argc, char *argv[])
// {
// 	WSADATA						wsd;
// 	SOCKET						sListen, sClient;
// 	struct sockaddr_in			local, client;
// 	int							iAddrSize = sizeof(client);
// 	fd_set						fdread;
// 	int							ret;
// 	unsigned long				hThread[MAX_THREAD];
// 	int							index = 0;
// 	THREAD_PARAM				param;
// 
// 	if(argc != 4)
// 	{
// 		printf("usage: ./a.out bindPort DstIP DstPort\n");
// 		return -1;
// 	}
// 
// 	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
// 	{
// 		printf("Failed to load winsock!\n");
// 		return -1;
//     }
// 
// 	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
// 	if (sListen == SOCKET_ERROR)
//     {
//         printf("socket() failed: %d\n", GetLastError());
//         return 1;
//     }
// 
// 	local.sin_addr.s_addr = htonl(INADDR_ANY);
//     local.sin_family = AF_INET;
//     local.sin_port = htons(atoi(argv[1]));
// 	
// 	if (bind(sListen, (struct sockaddr *)&local, 
// 		sizeof(local)) == SOCKET_ERROR)
//     {
//         printf("bind() failed: %d\n", GetLastError());
//         return 1;
//     }
// 	
//     listen(sListen, 5);
// 	
// 	printf("开始监听端口[%d]\n", atoi(argv[1]));
// 
// 	while (1)
// 	{
// 		FD_ZERO(&fdread);
// 		FD_SET(sListen, &fdread);
// 
// 		if((ret =select(0, &fdread, NULL, NULL, NULL)) == SOCKET_ERROR)
// 		{
// 			printf("select出了不该出的错了[%d]\n", GetLastError());
// 			return -2;
// 		}
// 		//in this case the ret can only for 1
// 		if(ret > 0)
// 		{
// 			if(FD_ISSET(sListen, &fdread))
// 			{
// 				sClient = accept(sListen, (struct sockaddr *)&client,
// 								&iAddrSize); 
// 				if(sClient == SOCKET_ERROR)
// 				{
// 					printf("accept Error[%d]\n",GetLastError());
// 					return -3;
// 				}
// 				printf("via listenPort[%d] accept [%s:%d]\n", 
// 					atoi(argv[1]), 
// 					inet_ntoa(client.sin_addr), 
// 					ntohs(client.sin_port));
// 
// 				/**fill in the thread param*/
// 				param.cliSock = sClient;
// 				strncpy(param.DstIP, argv[2], sizeof(param.DstIP));
// 				param.DstPort = atoi(argv[3]);
// 				//create a thread to deal with this client
// 				hThread[index] = _beginthreadex(NULL, 0, dealClientThread, (void* )&param, 0, NULL);
// 				if(hThread[index] != NULL)
// 					++index;
// 			}
// 		}
// 	}
// 
// 	return 0;
// }
/**
*	connDst	-	[in]	阻塞连接Srv
*	@dstIP:		[in]	连接的服务器IP
*	@dstPort:	[in]	连接的端口
*	@srvSock:	[in/out]返回连接到服务端的socket
*	return
*		==	0: 成功
*		!=	0:失败
*/
// int connDst(char *dstIP, unsigned short dstPort, SOCKET	&srvSock)
// {
// 	struct sockaddr_in				addrServer;
// 
// 	srvSock = socket(AF_INET, SOCK_STREAM,0);
// 	if(srvSock == INVALID_SOCKET)
//     {
// 		return GetLastError();
//     }
// 
// 	addrServer.sin_family = AF_INET;
// 	addrServer.sin_addr.s_addr = inet_addr(dstIP);
// 	addrServer.sin_port = htons(dstPort);
// 
// 	if(connect(srvSock,(const struct sockaddr *)&addrServer,sizeof(sockaddr)) != 0)
//     {
// 		closesocket(srvSock);
//         return GetLastError();
//     }
// 
// 	return 0;
// }

/**
 *	setSockBuf	-	设置套接字的发送缓冲区与接收缓冲区
 *
 *	@sock:		[in]	需要设置的套接字
 *	return
 *				==0		成功
 *				< 0	失败
 */
int setSockBuf(SOCKET sock)
{
	int			optVal = SOCKET_BUFF_SIZE;
	int			optLen = sizeof(int);

	/** set recv buffer size*/
	if(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, optLen) == SOCKET_ERROR)	
		return BUILD_ERROR(_OSerrno(), EOS);
	/** set send buffer size*/
	if(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&optVal, optLen) == SOCKET_ERROR) 
		return BUILD_ERROR(_OSerrno(), EOS);

	return 0;
}

/**
 *	setSockUnBlock	-	设置套接字为非阻塞
 *
 *	@sock:		[in]	需要设置的套接字
 *	return
 *				==0		成功
 *				<0	失败
 */
int setSockUnBlock(SOCKET sock)
{
	unsigned long	arg = 1;
	
	if(ioctlsocket(sock, FIONBIO, (unsigned long*)&arg) == SOCKET_ERROR)
		return BUILD_ERROR(_OSerrno(), ESETSOCK);
	
	return 0;
}

/**
 *	init		-			对两端套接字及select的超时时间进行初始化
 *
 *	@sock1:		[in]		需要设置的第一个套接字
 *	@sock2:		[in]		需要设置的第二个套接字
 *	@tv			[in/out]	需要设置的select的超时时间
 *
 *	return
 *				==0			成功
 *				<0			失败
 */
int init(SOCKET sock1, SOCKET sock2, struct timeval &tv)
{	
	int						ret;

	INIT_TIMEVAL(tv, SELECT_TIMEOUT);

	if((ret = setSockBuf(sock1)) < 0)
		return ret;
	if((ret = setSockBuf(sock2)) < 0)
		return ret;
// 	if((ret = setSockUnBlock(sock1)) < 0)
// 		return ret;
// 	if((ret = setSockUnBlock(sock2)) < 0)
// 		return ret;
	
	return 0;
}

/**
 *	sendData -	非阻塞发送数据
 *
 *	@hs			[in]	操作数
 *	
 *	return
 *		==0		正常，数据全部发送出去了
 *		<0		出错，或者数据没有全部发送出去
 */
int sendToPeer(SOCKET sock, char *sendBuffer, int size)
{
	if(send(sock, sendBuffer, size, 0) != size)
		return -1;

	return 0;
}


/**
 *	cliToSrv -		从客户端向服务端转发数据
 *
 *	@hs			[in]	操作数
 *	
 *	return
 *		==0		正常，不需要关闭套接字
 *		<0		出错(包括有套接字关闭)
 */
int cliToSrv(SOCK_INFO &hs)
{
	int					ret;
	char				*pSend = NULL;
	
	ret = recv(hs.sock, hs.buffer+MAX_HTTPHEAD_SIZE, MAX_IOBUFFER_LEN, 0);
	if(ret <= 0)
		return BUILD_ERROR(_OSerrno(), ECCLOSE);

	ret = buildHttpData(hs.buffer+MAX_HTTPHEAD_SIZE,
						ret,
						MAX_HTTPHEAD_SIZE,
						&pSend,
						hs.usUnitSerial,
						hs.ulLinkNo);
	if(ret < 0)
		return ret;
	
	ret = sendToPeer(hs.peerSock, pSend, ret);
	if(ret != 0)
		return BUILD_ERROR(_OSerrno(), ESCLOSE);

	return 0;
}


void initState(HTTP_PARASE_PARAM &httpParam, LINE_PARASE_PARAM &headParam)
{
	httpParam.state = HTTP_PARASE_HEADER;
	headParam.state = HTTP_REQUEST;
}

/** need ajusted*/
int srvToCli(SOCK_INFO &hs)
{	
	unsigned int			&leaveParased = hs.engine.leaveParased;
	HTTP_PARASE_PARAM		&httpParam = hs.engine.httpParase;
	LINE_PARASE_PARAM		&headParam = hs.engine.lineParase;	
	int						onceRecved = 0;
	int						ret;
	
	onceRecved = recv(hs.sock, hs.buffer+leaveParased, sizeof(hs.buffer)-leaveParased, 0);
	if(onceRecved <= 0)
		return BUILD_ERROR(_OSerrno(), ESCLOSE);
	/** do something with hs.HTTP_PARASE_ENG*/
	//ret = paraseHttpData(hs,);
	ret = httpParase(hs.buffer, onceRecved+leaveParased, httpParam, headParam, hs.peerSock);
	assert(ret <= onceRecved+leaveParased);
	if(ret < 0)
	{
		printf("解析出错\n");
		return ret;
	}

	if(httpParam.state == HTTP_PARASE_OK)
	{
		initState(httpParam, headParam);
// 		ret = sendToPeer(hs.peerSock, hs.buffer, ret);/** can send 0 bi*/
// 		if(ret != 0)
// 			return BUILD_ERROR(_OSerrno(), ECCLOSE);
	}


	leaveParased = onceRecved+leaveParased-ret;

	memmove(hs.buffer, hs.buffer+ret, leaveParased);

	return 0;
}

/**
 *	trsData -		对数据进行转发
 *
 *	@lhs			[in]	操作数
 *	
 *	return
 *		==0		正常，不需要关闭套接字
 *		<0		出错(包括有一端套接字关闭)
 */
int _trsData(SOCK_INFO &hs)
{
	if(IS_CLI(hs))
		return cliToSrv(hs);
	else
		return srvToCli(hs);
}
/**
 *	trsData -		对数据进行转发
 *
 *	@fdRead:		[in]		可读套接字集合
 *	@lhs			[in/out]	左操作数
 *	@rhs			[in/out]	右操作数
 *	
 *
 *	return
 *		==0		正常，不需要关闭套接字
 *		<0		出错(包括有一端套接字关闭)
 */
int trsData(fd_set &fdRead, SOCK_INFO &lhs, SOCK_INFO &rhs)
{
	int						ret;
	for( unsigned int i = 0; i < fdRead.fd_count; ++i )
	{	
		if(fdRead.fd_array[i] == lhs.sock)
		{
			ret = _trsData(lhs);
			if(ret != 0)	
				return ret;
		}else if(fdRead.fd_array[i] == rhs.sock)
		{
			ret = _trsData(rhs);
			if(ret != 0)
				return ret;
		}
		else
		{
			return BUILD_ERROR(0, EABORT);
		}
	}

	return 0;
}

/**
 *	select2sock -		用select管理两套接字
 *
 *	@cliSockInfo:	[in/out]	已建立好连接的与srv的套接字相关信息
 *	@srvSockInfo:	[in/out]	已建立好连接的与cli的套接字相关信息
 *	
 *
 *	return
 *		<0		错误码里包含具体原因(有端关闭?用户取消线程)
 */
int select2sock(SOCK_INFO &cliSockInfo, SOCK_INFO &srvSockInfo)
{
	struct timeval			tv;					/** the timeout of select*/
	fd_set					fdRead;				/** the fd_set*/
	unsigned long			toSrvBytes = 0;		/** transmit from cli to srv bytes*/
	unsigned long			toCliBytes = 0;		/** transmit from srv to cli bytes*/				
	int						ret;

	if(ret = init(cliSockInfo.sock, srvSockInfo.sock, tv) < 0)
		return ret;

	while(1)
	{
		if(USER_CANCLE)	/** cancel this thread*/
			return BUILD_ERROR(0, EFASTCLOSE);

		INIT_SELECT(fdRead, cliSockInfo.sock, srvSockInfo.sock);
		ret = select(0, &fdRead, NULL, NULL, &tv);
		switch (ret)
		{
		case SOCKET_ERROR:		/** some error happen*/
			ret = BUILD_ERROR(_OSerrno(), ESUCESS);
			break;
		case 0:					/** select timeout*/
			ret = 0;
			break;
		default:				/** need to look for fdRead and transmit*/
			ret = trsData(fdRead, cliSockInfo, srvSockInfo);
			break;
		}

		if(ret < 0)
			return ret;
	}
}

void initSockInfo(SOCK_INFO &cliSockInfo, SOCKET cliSock, SOCK_INFO &srvSockInfo, SOCKET srvSock)
{
	cliSockInfo.sock = cliSock;
	cliSockInfo.peerSock = srvSock;
	cliSockInfo.towards = 0;
	srvSockInfo.sock = srvSock;
	srvSockInfo.towards = 1;
	srvSockInfo.peerSock = cliSock;
	INIT_PARASE_ENG(cliSockInfo.engine);
	INIT_PARASE_ENG(srvSockInfo.engine);
}

void fastCloseSock(SOCKET sock)
{
	struct linger opt;
	
	opt.l_onoff	= 1;
	opt.l_linger= 0;

	setsockopt(sock, SOL_SOCKET, SO_LINGER,
		(const char*)&opt, sizeof(opt));

	closesocket(sock);
}
void close2socket(SOCKET sock1, SOCKET sock2, bool fastClsose)
{
	if(fastClsose)
	{
		fastCloseSock(sock1);
		fastCloseSock(sock2);
	}else
	{
		closesocket(sock1);
		closesocket(sock2);
	}
}
/**
 *	dealClientThread - 处理一个客户端
 *
 *	处理流程：向DST进行连接-> 利用select对DST与CLI之间的数据进行转发
 *	@lpParameter:	线程参数
 *	
 *
 *	return
 *		==0:	成功
 *		!=0:	出错
 */
int dealClient(SOCKET cliSock, SOCKET srvSock)
{
	SOCK_INFO				cliSockInfo, srvSockInfo;
	int						ret;
	
	
	initSockInfo(cliSockInfo, cliSock, srvSockInfo, srvSock);
	
	ret = select2sock(cliSockInfo, srvSockInfo);
	if(NEED_FAST_CLOSE(ret)){
		close2socket(cliSock, srvSock, true);/** fast close*/
		ret = 0;
	}
	else
		close2socket(cliSock, srvSock, false);/** normal close*/

	printf("End select2sock return sysError[%d] userError[%d]\n", 
		GET_SYS_ERR(ret), GET_USER_ERR(ret));

	return ret;
}