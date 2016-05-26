#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>

//#define DEFAULT_PORT        5678
#define DEFAULT_BUFFER      81920//让其大点

typedef struct _PARAM
{
	unsigned short			bindPort;
	unsigned char			isEchoSrv;
	long					bigDataSize;
	int						perSize;
} PARAM;



int send_data( int sockfd, char* buf, int size )
{
	int		rsize = 0;
	int		errorcode;
	while ( rsize != size )
	{
		errorcode = send( sockfd, buf + rsize, size - rsize, 0 );
		if ( errorcode == SOCKET_ERROR )
		{
			printf("send Error[%d]\n",GetLastError());
			errorcode =  GetLastError();
			return	errorcode;
		}
		
		rsize += errorcode;
	}
	
	return	0;
}

int echo(SOCKET sock, int index)
{
	char					tmpBuffer[DEFAULT_BUFFER];
	int						errCode;
	long					totalEchoSize = 0;

	while(1)
	{
		if((errCode = recv(sock, tmpBuffer, DEFAULT_BUFFER, 0)) == SOCKET_ERROR )
		{
			printf("第[%d]客户端，收数据发生错误[%d]\n", index, GetLastError());
			
			return -1;
		}

		if(errCode == 0)
		{
			printf("第[%d]客户端，客户端已经关闭了套接字\n", index);
			
			return -2;
		}
		if(send_data(sock, tmpBuffer, errCode) != 0)
		{
			printf("第[%d]客户端，发数据出错[%d]\n", index, GetLastError());

			return -3;
		}

		totalEchoSize += errCode;

		printf("第[%d]客户端，这次回显了[%d]字节，总共回显[%d]字节\n", index, errCode, totalEchoSize);
	}

	return 0;
}

int sendBigData(SOCKET sClient, int index, PARAM &param)
{
	char				recvBuffer[DEFAULT_BUFFER];
	long				needSendSize = param.bigDataSize;
	int					perSendSize = param.perSize;
	long				hadSendSize = 0;
	char				*pSendBuffer = NULL;
	int					errCode;
	int					onceSend;
	unsigned int		recvTimeOut = 5000;//5秒


	if(setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeOut, sizeof(unsigned int)) != 0)
	{
		printf("设置套接字超时时间出错\n");
		return -1;
	}

// 	if(recv(sClient, recvBuffer, DEFAULT_BUFFER, 0) == SOCKET_ERROR)
// 	{
// 		printf("recv Error[%d]\n", GetLastError());
// 
// 		return -2;
// 	}
	
	if((pSendBuffer = (char* )malloc(needSendSize)) == NULL)
	{
		printf("第[%d]客户端，malloc [%d]字节失败\n", index, needSendSize);

		return -3;
	}

	printf("第[%d]客户端，开始向客户端发[%d]字节\n", index, needSendSize);

	while(1)
	{
		onceSend = min(needSendSize, perSendSize);
		errCode = send_data(sClient, pSendBuffer+hadSendSize, onceSend);
		if(errCode != 0)
		{
			printf("第[%d]客户端，发数据出错[%d]\n", index, errCode);
			errCode = -4;
			goto end;
		}
		hadSendSize += onceSend;
		needSendSize -= onceSend;
		printf("第[%d]客户端，此次发了[%d]字节，总共已经发了[%d]字节\n", index, onceSend, hadSendSize);
		
		if(needSendSize <= 0)
			break;
	}
	printf("第[%d]客户端，总共向客户端发送了[%d]\n",index, hadSendSize);

end:
	if(pSendBuffer != NULL)
	{
		free(pSendBuffer);
		pSendBuffer = NULL;
	}

	return errCode;
}

int doDeal(SOCKET sClient, int index, PARAM &param)
{
	int						err;

	switch (param.isEchoSrv)
	{
	case 0:
		err = sendBigData(sClient, index, param);
		break;
	case 1:
		err = echo(sClient, index);
		break;
	default:
		err = -1;
		break;
	}

	return err;
}

int main(int argc, char **argv)
{
    WSADATA       wsd;
    SOCKET        sListen, sClient;
    int           iAddrSize;
    struct sockaddr_in local, client;
	PARAM		param;

	if(argc != 5)
	{
		printf("usage: bindPort isEcho bigDataSize perSendSize\n");
		return -1;
	}

	param.bindPort = atoi(argv[1]);
	param.isEchoSrv = atoi(argv[2]);
	param.bigDataSize = atol(argv[3]);
	param.perSize = atoi(argv[4]);
	if(param.isEchoSrv > 1 || param.isEchoSrv < 0)
	{
		printf("isEcho 参数只能为0或者1\n");
		return -2;
	}

	if(param.isEchoSrv)
	{
		printf("本程序是回显服务器，收客户端的数据，然后原样返回给客户端\n");
	}else
	{
		printf("本程序，当收到客户端的连接后，不收数据，直接返回给客户端[%d]字节\n\t如果发的数据超过了[%d]字节,则每次发[%d]字节\n\n", param.bigDataSize, param.perSize, param.perSize);
	}

    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("Failed to load Winsock!\n");
        return -1;
    }

    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sListen == SOCKET_ERROR)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
        return -2;
    }
    
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(param.bindPort);

    if (bind(sListen, (struct sockaddr *)&local, 
            sizeof(local)) == SOCKET_ERROR)
    {
        printf("bind() failed: %d\n", WSAGetLastError());
        return 1;
    }
    listen(sListen, 5);
    
	int index =0;
	printf("开始监听,监听端口[%d]\n", param.bindPort);

    while (1)
    {
        iAddrSize = sizeof(client);
        sClient = accept(sListen, (struct sockaddr *)&client,
                        &iAddrSize);        
        if (sClient == INVALID_SOCKET)
        {        
            printf("accept() failed: %d\n", WSAGetLastError());
            break;
        }

        printf("第[%d]连接: %s:%d到达\n", ++index,
           inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		doDeal(sClient, index, param);

		closesocket(sClient);
		//echo(sClient, index);
    }

    closesocket(sListen);
   
    WSACleanup();
    return 0;
}
