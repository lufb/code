/**
 *	main.c
 *
 *	Copyright (C) 2013 ¬���� <1164830775@qq.com>
 *
 *	�޸���ʷ:
 *
 *	2013--11-08 - �״δ���
 *
 *  ��������:     
 *			ѧϰʹ��select(����ת��)
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
// 	printf("��ʼ�����˿�[%d]\n", atoi(argv[1]));
// 
// 	while (1)
// 	{
// 		FD_ZERO(&fdread);
// 		FD_SET(sListen, &fdread);
// 
// 		if((ret =select(0, &fdread, NULL, NULL, NULL)) == SOCKET_ERROR)
// 		{
// 			printf("select���˲��ó��Ĵ���[%d]\n", GetLastError());
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
*	connDst	-	[in]	��������Srv
*	@dstIP:		[in]	���ӵķ�����IP
*	@dstPort:	[in]	���ӵĶ˿�
*	@srvSock:	[in/out]�������ӵ�����˵�socket
*	return
*		==	0: �ɹ�
*		!=	0:ʧ��
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
 *	setSockBuf	-	�����׽��ֵķ��ͻ���������ջ�����
 *
 *	@sock:		[in]	��Ҫ���õ��׽���
 *	return
 *				==0		�ɹ�
 *				< 0	ʧ��
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
 *	setSockUnBlock	-	�����׽���Ϊ������
 *
 *	@sock:		[in]	��Ҫ���õ��׽���
 *	return
 *				==0		�ɹ�
 *				<0	ʧ��
 */
int setSockUnBlock(SOCKET sock)
{
	unsigned long	arg = 1;
	
	if(ioctlsocket(sock, FIONBIO, (unsigned long*)&arg) == SOCKET_ERROR)
		return BUILD_ERROR(_OSerrno(), ESETSOCK);
	
	return 0;
}

/**
 *	init		-			�������׽��ּ�select�ĳ�ʱʱ����г�ʼ��
 *
 *	@sock1:		[in]		��Ҫ���õĵ�һ���׽���
 *	@sock2:		[in]		��Ҫ���õĵڶ����׽���
 *	@tv			[in/out]	��Ҫ���õ�select�ĳ�ʱʱ��
 *
 *	return
 *				==0			�ɹ�
 *				<0			ʧ��
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
 *	sendData -	��������������
 *
 *	@hs			[in]	������
 *	
 *	return
 *		==0		����������ȫ�����ͳ�ȥ��
 *		<0		������������û��ȫ�����ͳ�ȥ
 */
int sendToPeer(SOCKET sock, char *sendBuffer, int size)
{
	if(send(sock, sendBuffer, size, 0) != size)
		return -1;

	return 0;
}


/**
 *	cliToSrv -		�ӿͻ���������ת������
 *
 *	@hs			[in]	������
 *	
 *	return
 *		==0		����������Ҫ�ر��׽���
 *		<0		����(�������׽��ֹر�)
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
		printf("��������\n");
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
 *	trsData -		�����ݽ���ת��
 *
 *	@lhs			[in]	������
 *	
 *	return
 *		==0		����������Ҫ�ر��׽���
 *		<0		����(������һ���׽��ֹر�)
 */
int _trsData(SOCK_INFO &hs)
{
	if(IS_CLI(hs))
		return cliToSrv(hs);
	else
		return srvToCli(hs);
}
/**
 *	trsData -		�����ݽ���ת��
 *
 *	@fdRead:		[in]		�ɶ��׽��ּ���
 *	@lhs			[in/out]	�������
 *	@rhs			[in/out]	�Ҳ�����
 *	
 *
 *	return
 *		==0		����������Ҫ�ر��׽���
 *		<0		����(������һ���׽��ֹر�)
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
 *	select2sock -		��select�������׽���
 *
 *	@cliSockInfo:	[in/out]	�ѽ��������ӵ���srv���׽��������Ϣ
 *	@srvSockInfo:	[in/out]	�ѽ��������ӵ���cli���׽��������Ϣ
 *	
 *
 *	return
 *		<0		���������������ԭ��(�ж˹ر�?�û�ȡ���߳�)
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
 *	dealClientThread - ����һ���ͻ���
 *
 *	�������̣���DST��������-> ����select��DST��CLI֮������ݽ���ת��
 *	@lpParameter:	�̲߳���
 *	
 *
 *	return
 *		==0:	�ɹ�
 *		!=0:	����
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