#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>

//#define DEFAULT_PORT        5678
#define DEFAULT_BUFFER      81920//������

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
			printf("��[%d]�ͻ��ˣ������ݷ�������[%d]\n", index, GetLastError());
			
			return -1;
		}

		if(errCode == 0)
		{
			printf("��[%d]�ͻ��ˣ��ͻ����Ѿ��ر����׽���\n", index);
			
			return -2;
		}
		if(send_data(sock, tmpBuffer, errCode) != 0)
		{
			printf("��[%d]�ͻ��ˣ������ݳ���[%d]\n", index, GetLastError());

			return -3;
		}

		totalEchoSize += errCode;

		printf("��[%d]�ͻ��ˣ���λ�����[%d]�ֽڣ��ܹ�����[%d]�ֽ�\n", index, errCode, totalEchoSize);
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
	unsigned int		recvTimeOut = 5000;//5��


	if(setsockopt(sClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeOut, sizeof(unsigned int)) != 0)
	{
		printf("�����׽��ֳ�ʱʱ�����\n");
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
		printf("��[%d]�ͻ��ˣ�malloc [%d]�ֽ�ʧ��\n", index, needSendSize);

		return -3;
	}

	printf("��[%d]�ͻ��ˣ���ʼ��ͻ��˷�[%d]�ֽ�\n", index, needSendSize);

	while(1)
	{
		onceSend = min(needSendSize, perSendSize);
		errCode = send_data(sClient, pSendBuffer+hadSendSize, onceSend);
		if(errCode != 0)
		{
			printf("��[%d]�ͻ��ˣ������ݳ���[%d]\n", index, errCode);
			errCode = -4;
			goto end;
		}
		hadSendSize += onceSend;
		needSendSize -= onceSend;
		printf("��[%d]�ͻ��ˣ��˴η���[%d]�ֽڣ��ܹ��Ѿ�����[%d]�ֽ�\n", index, onceSend, hadSendSize);
		
		if(needSendSize <= 0)
			break;
	}
	printf("��[%d]�ͻ��ˣ��ܹ���ͻ��˷�����[%d]\n",index, hadSendSize);

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
		printf("isEcho ����ֻ��Ϊ0����1\n");
		return -2;
	}

	if(param.isEchoSrv)
	{
		printf("�������ǻ��Է��������տͻ��˵����ݣ�Ȼ��ԭ�����ظ��ͻ���\n");
	}else
	{
		printf("�����򣬵��յ��ͻ��˵����Ӻ󣬲������ݣ�ֱ�ӷ��ظ��ͻ���[%d]�ֽ�\n\t����������ݳ�����[%d]�ֽ�,��ÿ�η�[%d]�ֽ�\n\n", param.bigDataSize, param.perSize, param.perSize);
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
	printf("��ʼ����,�����˿�[%d]\n", param.bindPort);

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

        printf("��[%d]����: %s:%d����\n", ++index,
           inet_ntoa(client.sin_addr), ntohs(client.sin_port));

		doDeal(sClient, index, param);

		closesocket(sClient);
		//echo(sClient, index);
    }

    closesocket(sListen);
   
    WSACleanup();
    return 0;
}
