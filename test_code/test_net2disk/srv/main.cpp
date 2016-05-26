#include <stdio.h>
#include <winsock2.h>

#define BIND_PORT	7890

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
	SOCKET						sListen, sClient;
	struct sockaddr_in			local, client;
	int							index = 0;

	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
        return 1;
    }

	local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(BIND_PORT);
	
	if (bind(sListen, (struct sockaddr *)&local, 
		sizeof(local)) == SOCKET_ERROR)
    {
        printf("bind() failed: %d\n", WSAGetLastError());
        return 1;
    }
	
    listen(sListen, 5);
	
	printf("begin to listen [%d] port\n", BIND_PORT);

	while(1)
	{
		int iAddrSize = sizeof(client);
        sClient = accept(sListen, (struct sockaddr *)&client,
			&iAddrSize);        
        if (sClient == INVALID_SOCKET)
        {        
            printf("accept() failed: %d\n", WSAGetLastError());
            break;
        }
		printf("第[%d]SOCKET[%d]到达\n", index++, sClient);
        printf("新连接: %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	}

	return 0;
}