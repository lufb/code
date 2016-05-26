#include <stdio.h>
#include <winsock2.h>

#define DST_IP		"127.0.0.1"
#define DST_PORT	5678

void init()
{
	WSADATA       wsd;
	
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("Failed to load Winsock!\n");
    }
}

int main()
{
	SOCKET						sock;
	struct	sockaddr_in			addrServer;
	
	init();
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == INVALID_SOCKET)
    {
		printf("socket Error[%d]\n",GetLastError());
		return -1;
    }
	
	addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr(DST_IP);
    addrServer.sin_port = htons(DST_PORT);
	
	if(connect(sock,(const struct sockaddr *)&addrServer,sizeof(sockaddr)) != 0)
    {
        printf("connect[%s:%d] failed with[%d]\n", DST_IP, DST_PORT, GetLastError());
        return -2;
    }
	printf("connect [%s:%d] sucess\n", DST_IP, DST_PORT);


	return 0;
}