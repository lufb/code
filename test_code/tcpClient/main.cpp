#include <stdio.h>
#include <winsock2.h>

#define DST_IP		"192.168.1.99"
#define DST_PORT	10000

void init()
{
	WSADATA       wsd;
	
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("Failed to load Winsock!\n");
    }
}

int my_connect()
{
	SOCKET						sock;
	struct	sockaddr_in			addrServer;
	
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
        return -2;
    }
	
	return 0;
}

int main()
{
	unsigned int	i;
	int				err;
	init();

	for(i = 0; i < 130; ++i){
		err = my_connect();
		if(err == 0){
			printf("[%d]connect [%s:%d] sucess\n", i, DST_IP, DST_PORT);
		}else
		{
			printf("[%d]connect [%s:%d] failed[%d]\n", i, DST_IP, DST_PORT, GetLastError());
		}

		Sleep(100);
	}

	scanf("%d", &err);

	return 0;
}