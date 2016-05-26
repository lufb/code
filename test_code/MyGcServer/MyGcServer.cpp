// MyGcServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include "SrvMain.h"
#include "CliMain.h"
#include "Base.h"




int init();
//////////////////////////////////////////////////////////////////////////


int init()
{
	 WSADATA       wsd;

	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("Failed to load Winsock!\n");
        return 1;
    }

	return 0;
}

int main(int argc, char* argv[])
{
	SOCKET						sListen, sClient;
	struct sockaddr_in			local, client;
	HANDLE						hThreadCli, hThreadSrv;
	THREAD_PARAM				*pThreadParam = NULL;
	int							index = 0;

	init();
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
        return 1;
    }

	local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(DEFAULT_PORT);

	if (bind(sListen, (struct sockaddr *)&local, 
		sizeof(local)) == SOCKET_ERROR)
    {
        printf("bind() failed: %d\n", WSAGetLastError());
        return 1;
    }

    listen(sListen, 5);

	printf("��ʼ����[%d]�˿�\n", DEFAULT_PORT);

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
		printf("-------------------------------��[%d]SOCKET[%d]����\n", index, sClient);
        printf("������: %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

//
		pThreadParam = (THREAD_PARAM *)malloc(sizeof(THREAD_PARAM));
		if(NULL == pThreadParam)
		{
			printf("malloc error[%d]", GetLastError());
			break;
		}

		memset(pThreadParam, 0, sizeof(THREAD_PARAM));
		sprintf(pThreadParam->cliIP, inet_ntoa(client.sin_addr));
		pThreadParam->cliPort = ntohs(client.sin_port);
		pThreadParam->index = index++;
		pThreadParam->cliSocket = sClient;
		pThreadParam->isConnected = 0;
		pThreadParam->srvIP[0] = '\0';
		pThreadParam->srvPort = 0;
		pThreadParam->srvSocket = INVALID_SOCKET;
//		InitializeCriticalSection(&pThreadParam->lock);//��ʼ����
		pThreadParam->ulLinkNo = SRVCOMM_INVALID_LINKNO;
		pThreadParam->usUnitSerial = -1;
		pThreadParam->isTrade = false;			//add for trade
		pThreadParam->tradeState = TS_AUTH;		//add for trade
		pThreadParam->hadUsed = 0;	//add for trade
//
		//����S��
		hThreadSrv = CreateThread(NULL, 0, SrvThread, 
			(LPVOID)pThreadParam, 0, NULL);
        if (hThreadSrv == NULL)
        {
            printf("CreateThread() failed: %d\n", GetLastError());
            break;
        }

		//����C��
		hThreadCli = CreateThread(NULL, 0, ClientThread, 
			(LPVOID)pThreadParam, 0, NULL);
        if (hThreadCli == NULL)
        {
            printf("CreateThread() failed: %d\n", GetLastError());
            break;
        }

		CloseHandle(hThreadCli);
        CloseHandle(hThreadSrv);
	}
	
	closesocket(sListen);
	WSACleanup();

	return 0;
}







