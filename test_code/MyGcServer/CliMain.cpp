#include "StdAfx.h"
#include "CliMain.h"
#include "Base.h"
#include <assert.h>
#include "MHttp.h"


DWORD WINAPI ClientThread(LPVOID lpParam)
{
	THREAD_PARAM				*param = (THREAD_PARAM *)lpParam;
	int							recvSize;
	char						recvBuffer[MAX_FRAME];
	char						sendBuffer[MAX_FRAME];
	int							nRes;

	assert(param != NULL);
	//1 先等待向上连接建立成功
	while(1)
	{
		Sleep(500);
		if(param->isConnected == 1)//向上连接成功
		{
			break;
		}
	}

	//2 一直收数据，收到数据转发给客户端
	while(1)
	{
		if(param->srvSocket == INVALID_SOCKET)
		{
			printf("该套接字已经关闭了\n");
			return -1;
		}

		memset(recvBuffer, '\0', sizeof(recvBuffer));
		recvSize = ::recv(param->srvSocket, recvBuffer, MAX_FRAME, 0);
		if(recvSize <= 0)
		{
			printf("Cli:CLOSE####################[%d]\n",GetLastError());
			if( (nRes = MHttp::BuildClose(sendBuffer, MAX_FRAME, param->usUnitSerial, param->ulLinkNo)) < 0)
			{
				printf("Cli组关闭包发生错误[%d]\n", nRes);
				goto End;
			}

			if((nRes = ::send(param->cliSocket, sendBuffer, nRes, 0)) != nRes)
			{
				printf("Cli:向下发关闭包出错[%d]\n", GetLastError());
			}

			break;//跳出去，关闭相应套接字
		}else	//收到数据，应该往下丢数据
		{
			if(param->cliSocket == INVALID_SOCKET)
			{
				printf("Cli客户端的套接字已经被关闭\n");
				break;//跳出去，关闭相应套接字
			}
			
			memset(sendBuffer, '\0', sizeof(sendBuffer));
			if( ( nRes = MHttp::BuildData(sendBuffer,MAX_FRAME, param->usUnitSerial, param->ulLinkNo, recvBuffer, recvSize)) < 0 )
			{
				printf("Cli:组包发生错误[%d]\n", nRes);
				break;//跳出去，关闭相应套接字
			}

			printf("Cli:向下转发数据[%d]字节\n", recvSize);
			//向客户端转发数据
			if((nRes = ::send(param->cliSocket, sendBuffer, nRes, 0)) != nRes)
			{
				printf("Cli:向下发数据出错[%d]\n", GetLastError());
				break;//跳出去，关闭相应套接字
			}
		}
	}

End:
	//套接字资源的回收，内存暂不用考虑去释放
	if(param->srvSocket != INVALID_SOCKET)
	{
		printf("Cli:关闭向上套接字\n");
		closesocket(param->srvSocket);
		param->srvSocket = INVALID_SOCKET;
	}
	if(param->cliSocket != INVALID_SOCKET)
	{
		printf("Cli:关闭向下套接字\n");
		closesocket(param->cliSocket);
		param->cliSocket = INVALID_SOCKET;
	}
//为了测试，不让线程退出
	while(1)
	{
		Sleep(500);
	}

	return 0;
}