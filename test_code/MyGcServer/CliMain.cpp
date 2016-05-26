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
	//1 �ȵȴ��������ӽ����ɹ�
	while(1)
	{
		Sleep(500);
		if(param->isConnected == 1)//�������ӳɹ�
		{
			break;
		}
	}

	//2 һֱ�����ݣ��յ�����ת�����ͻ���
	while(1)
	{
		if(param->srvSocket == INVALID_SOCKET)
		{
			printf("���׽����Ѿ��ر���\n");
			return -1;
		}

		memset(recvBuffer, '\0', sizeof(recvBuffer));
		recvSize = ::recv(param->srvSocket, recvBuffer, MAX_FRAME, 0);
		if(recvSize <= 0)
		{
			printf("Cli:CLOSE####################[%d]\n",GetLastError());
			if( (nRes = MHttp::BuildClose(sendBuffer, MAX_FRAME, param->usUnitSerial, param->ulLinkNo)) < 0)
			{
				printf("Cli��رհ���������[%d]\n", nRes);
				goto End;
			}

			if((nRes = ::send(param->cliSocket, sendBuffer, nRes, 0)) != nRes)
			{
				printf("Cli:���·��رհ�����[%d]\n", GetLastError());
			}

			break;//����ȥ���ر���Ӧ�׽���
		}else	//�յ����ݣ�Ӧ�����¶�����
		{
			if(param->cliSocket == INVALID_SOCKET)
			{
				printf("Cli�ͻ��˵��׽����Ѿ����ر�\n");
				break;//����ȥ���ر���Ӧ�׽���
			}
			
			memset(sendBuffer, '\0', sizeof(sendBuffer));
			if( ( nRes = MHttp::BuildData(sendBuffer,MAX_FRAME, param->usUnitSerial, param->ulLinkNo, recvBuffer, recvSize)) < 0 )
			{
				printf("Cli:�����������[%d]\n", nRes);
				break;//����ȥ���ر���Ӧ�׽���
			}

			printf("Cli:����ת������[%d]�ֽ�\n", recvSize);
			//��ͻ���ת������
			if((nRes = ::send(param->cliSocket, sendBuffer, nRes, 0)) != nRes)
			{
				printf("Cli:���·����ݳ���[%d]\n", GetLastError());
				break;//����ȥ���ر���Ӧ�׽���
			}
		}
	}

End:
	//�׽�����Դ�Ļ��գ��ڴ��ݲ��ÿ���ȥ�ͷ�
	if(param->srvSocket != INVALID_SOCKET)
	{
		printf("Cli:�ر������׽���\n");
		closesocket(param->srvSocket);
		param->srvSocket = INVALID_SOCKET;
	}
	if(param->cliSocket != INVALID_SOCKET)
	{
		printf("Cli:�ر������׽���\n");
		closesocket(param->cliSocket);
		param->cliSocket = INVALID_SOCKET;
	}
//Ϊ�˲��ԣ������߳��˳�
	while(1)
	{
		Sleep(500);
	}

	return 0;
}