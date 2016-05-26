// ReUsePort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <time.h>

#define			REUSE_PORT				80

int  Bind(SOCKET stSocket,unsigned short lPort)
{
	struct sockaddr_in				sttempaddr;
	
	sttempaddr.sin_family = AF_INET;
	sttempaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sttempaddr.sin_port = htons(lPort);
	
	/*
	*	GUOGUO 20090826 增加监听SOCKET的LINUX的特性
	*/
#ifdef LINUXCODE
	unsigned int	optval;
	struct	 linger	optval1;
	//  设置SO_REUSEADDR选项(服务器快速重起)
	optval   =   0x1;
	setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, &optval, 4);
	
	//设置SO_LINGER选项(防范CLOSE_WAIT挂住所有套接字)
	optval1.l_onoff = 1;
	optval1.l_linger = 60;
	setsockopt(stSocket, SOL_SOCKET, SO_LINGER, &optval1, sizeof(struct linger));
#endif
	
	if ( bind(stSocket,(sockaddr *)&sttempaddr,sizeof(sttempaddr)) == SOCKET_ERROR )
	{
		printf("bind Error:%d\n",GetLastError());
		return -3;
	}
	
	return(1);
}

int main(int argc, char* argv[])
{
	WSADATA						wsd;

	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("Failed to load Winsock!\n");
        return -1;
    }
	
	SOCKET						sListen, sClient;
	int							listen_opt=1;
	struct sockaddr_in			client;
	int							iAddrSize = sizeof(client);
	time_t						tmp_time;

	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sListen == SOCKET_ERROR)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
        return -2;
    }

	setsockopt(sListen,SOL_SOCKET,SO_REUSEADDR,(const char*)&listen_opt,sizeof(&listen_opt));
	if(Bind(sListen, REUSE_PORT) != 1)
	{
		printf("Bind Error[%d]",GetLastError());
		return -3;
	}

	printf("该程序绑定的端口为[%d],开始监听客户端的连接\n", REUSE_PORT);

	listen(sListen, 50);

	while(1)
	{
		sClient = accept(sListen, (struct sockaddr *)&client, &iAddrSize);        
		if (sClient == INVALID_SOCKET)
		{        
			printf("accept() failed: %d\n", WSAGetLastError());
			continue;
		}
		time(&tmp_time);
		printf("在时间[%d]新连接到达,Socket[%d]\n", tmp_time, sClient);
	}

	WSACleanup();

	return 0;
}

