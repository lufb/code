// GCConnAndClosePreTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gree.h"
#include "GreeSocket.h"
#include "interface.h"
#include "GCConnAndClosePreTest.h"
#include <sys/stat.h>
#include <windows.h>

char	TEST_DATA[256];						//
int MAX_FRAME_SIZE = 0;						//

bool setConsoleColor(WORD wAttributes)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hConsole == INVALID_HANDLE_VALUE)
		return false;
	return SetConsoleTextAttribute(hConsole, wAttributes);
}
int Init()
{
    WSAData wsaData;
    if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
    {
		printf("WSAStartupʧ��\n");
        return -1;
    }
	
    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
		printf("SOCKET�汾����\n");
        WSACleanup();
        return -1;
    }
    return 0;
}

void printParam(PARAM &param)
{
	printf("DSTIP:%25s\n", param.DstIP);
	printf("DSTPort:%23d\n", param.DstPort);
	printf("GcSIP:%25s\n", param.GcIP);
	printf("GcPort:%24d\n", param.GcPort);
	printf("UseGc:%25d\n", param.useGc);
	printf("IsEcho:%24d\n", param.isEcho);
	printf("Repeat:%24d\n", param.repeatTime);
	printf("MaxFrameSize:%18d\n", param.maxFrameSize);
	printf("ParamFileName:%17s\n", param.testDataName);
	printf("AgentParamFile:%17s\n\n", param.AgentParamName);
}

long getFileSize(char* fileName)
{
	struct stat stUpDatetime; 
	if(stat(fileName,  &stUpDatetime) < 0)
	{
		printf("��ȡ�ļ�����ʧ�ܣ�GetLastError[%d]\n", GetLastError());
		return -1;
	}
	
	return stUpDatetime.st_size;
}

int fillParam(PARAM &param, int argc, char *argv[])
{
	assert(argc >= 8);

	printf("��������������:\n\n");

	strncpy(param.DstIP, argv[0], MAX_IP_LEN);
	param.DstPort = atoi(argv[1]);
	strncpy(param.GcIP, argv[2], MAX_IP_LEN);
	param.GcPort = atoi(argv[3]);
	param.useGc = atoi(argv[4]);
	param.isEcho = atoi(argv[5]);
	param.repeatTime = atoi(argv[6]);
	param.maxFrameSize = atoi(argv[7]);
	strncpy(param.testDataName, argv[8], sizeof(param.testDataName));
	strncpy(param.AgentParamName, argv[9], sizeof(param.AgentParamName));

	MAX_FRAME_SIZE = param.maxFrameSize;//
	strncpy(TEST_DATA, param.testDataName, sizeof(TEST_DATA));


	if(param.useGc < 0 || param.useGc > 1)
	{
		printf("�Ƿ�����GcS����ֻ����0����1\n");
		return -1;
	}

	if(param.isEcho < 0 || param.isEcho > 1)
	{
		printf("�Ƿ�����CRCУ�����ֻ����0����1\n");
		return -2;
	}

	return 0;
}

FILE* openAndGetFileSize(char *fileName, long &fileSize)
{
	FILE			*fp;
	
	fp = fopen(fileName, "rb");
	if(fp == NULL)
	{
		printf("���ļ�[%s]����[%d]\n", fileName, GetLastError());
		return NULL;
	}
	
	fileSize = getFileSize(fileName);
	if(fileSize < 0)
	{
		fclose(fp);
		return NULL;
	}

	return fp;
}

int _sendAndRecv(int index, SOCKET sock, char* pSendData, long size, char* pRecvData)
{
	int						errCode;

	if((errCode = send_data(sock, pSendData, size)) != 0)
	{
		SET_RED
		printf("send Error[%d]\n", errCode);
		SET_BACK
		return -1;
	}
	//printf("[%d]����[%d]�ֽ�\n", index ,size);

	if((errCode = recv_data(sock, pRecvData, size)) != size)
	{
		SET_RED
		printf("recv Error[%d]\n", errCode);
		SET_BACK
		return -2;
	}
	//printf("�յ�[%d]�ֽ�\n", errCode);

	return size;
}

int _doEcho(SOCKET sock, FILE *fp, long fileSize)
{
	assert(fp != NULL);

	char					*pSendData = NULL;
	char					*pRecvData = NULL;
	int						errCode;
	long					headDealed = 0;
	int						index = 0;
 
// 	unsigned long	arg = 1;
// 
// 	int rc = ioctlsocket(sock, FIONBIO, (unsigned long*)&arg);
// 	if(rc == SOCKET_ERROR)
// 		printf("�����׽��ַ���������\n");


	if((pSendData = (char* )malloc(fileSize)) == NULL)
	{
		SET_RED
		printf("malloc1 Error[%d]\n", GetLastError());
		SET_BACK
		errCode = -1;
		goto echoEnd;
	}

	if((pRecvData = (char* )malloc(fileSize)) == NULL)
	{
		SET_RED
		printf("malloc2 Error[%d]\n", GetLastError());
		SET_BACK
		errCode = -2;
		goto echoEnd;
	}

	if(fread(pSendData, sizeof(char), fileSize, fp) != fileSize)
	{
		SET_RED
		printf("fread Error[%d]\n", GetLastError());
		SET_BACK
		errCode = -3;
		goto echoEnd;
	}

	while(1)
	{
		errCode = _sendAndRecv(++index, sock, pSendData+headDealed, min(fileSize, MAX_FRAME_SIZE), pRecvData+headDealed);
		if(errCode < 0)
		{
			goto echoEnd;
		}
		
		
		headDealed += errCode;
		fileSize -= errCode;

		if(fileSize == 0)
			break;
	}

	if(memcmp(pSendData, pRecvData, headDealed) != 0)
	{
		SET_RED
		printf("������[%d]�ֽ�,������[%d]�ֽڣ������ݱ���\n");
		SET_BACK
		errCode = -4;
		goto echoEnd;
	}else{
		errCode = 0;
		goto echoEnd;
	}

echoEnd:
	if(pSendData != NULL)
	{
		free(pSendData);
		pSendData = NULL;
	}

	if(pRecvData != NULL)
	{
		free(pRecvData);
		pRecvData = NULL;
	}

	return errCode;
}

int recvNoEcho(SOCKET sock)
{
	//char				recvBuffer[MAX_FRAME_SIZE];
	char				*recvBuffer = (char* )malloc(MAX_FRAME_SIZE);
	int					errCode = 0;
	long				totalRecved = 0;
	int					i;

	printf("\n");
	while(1)
	{
		if((errCode = recv(sock, recvBuffer, MAX_FRAME_SIZE, 0)) == SOCKET_ERROR )
		{
			SET_RED
			printf("�շǻ������ݳ���[%d],���ڲ��յ�[%d]�ֽ�\n", GetLastError(), totalRecved);
			SET_BACK
			return errCode;
		}
		if(errCode == 0)
		{
			printf("\n\n�ǻ�����������ɣ��ܹ��յ�[%d]�ֽ�\n", totalRecved);
			return 0;
		}
		totalRecved += errCode;
		for(i = 0; i < errCode; ++i)
		{
			printf("%c", recvBuffer[i]);
		}
	}
	
	
	return 0;
}

int _doNoEcho(SOCKET sock, FILE *fp, long fileSize, int index)
{
	char					*pSendData = NULL;
	int						errCode = 0;

	if((pSendData = (char* )malloc(fileSize)) == NULL)
	{
		SET_RED
		printf("malloc1 Error[%d]\n", GetLastError());
		SET_BACK
		errCode = -1;
		goto noEchoEnd;
	}

	if(fread(pSendData, sizeof(char), fileSize, fp) != fileSize)
	{
		SET_RED
		printf("fread Error[%d]\n", GetLastError());
		SET_BACK
		errCode = -2;
		goto noEchoEnd;
	}

	if(fileSize > MAX_FRAME_SIZE)
	{
		SET_RED
		printf("�ǻ���ģʽ�£��㷢������Ϊ[%d]���ѳ�������������ˮƽ����ֻ����ת��[%d],��֪��\n", fileSize, MAX_FRAME_SIZE);
		SET_BACK
		fileSize = MAX_FRAME_SIZE;
	}

// 	if((errCode = send_data(sock, pSendData, fileSize)) != 0)
// 	{
// 		SET_RED
// 		printf("�ǻ��Է������ݳ���[%d]\n", errCode);
// 		SET_BACK
// 		errCode = -3;
// 		goto noEchoEnd;
// 	}

	if((errCode = recvNoEcho(sock)) != 0)
	{
		errCode = -4;
		goto noEchoEnd;
	}

noEchoEnd:
	if(pSendData != NULL)
	{
		free(pSendData);
		pSendData = NULL;
	}
	
	return errCode;
}

int doEcho(SOCKET sock, FILE *fp, long fileSize, unsigned short index)
{
	printf("��ʼ��[%d]�λ��ԵĲ���\n", index);
	return  _doEcho(sock, fp, fileSize);
}


int doNoEcho(SOCKET sock, FILE *fp, long fileSize, int index)
{	
	printf("��ʼ��[%d]�β��ԣ��ǻ��ԵĲ��Կ�ʼ\n", index);	
	return _doNoEcho(sock, fp, fileSize, index);
}
//ֱ�Ӱ��ļ�������һ���Զ����ڴ��У�Ȼ��������
int sendAndRecv(SOCKET sock, char isEcho, int index)
{
	long				fileSize;
	char				*pData = NULL;
	FILE				*fp;
	int					errCode;

	fp = openAndGetFileSize(TEST_DATA, fileSize);
	if(fp == NULL)
		return -1;

	switch(isEcho)
	{
	case 0:
		errCode = doNoEcho(sock, fp, fileSize, index);
		break;
	case 1:
		errCode = doEcho(sock, fp, fileSize, index);
		break;
	default:
		break;
	}

	fclose(fp);

	return errCode;
}

int _doTest(PARAM &param, SOCKET sock, int index)
{
	return sendAndRecv(sock, param.isEcho,  index);
}

SOCKET doDirSocket(PARAM &param)
{
	SOCKET							sock;
	struct							sockaddr_in addrServer;
	int								errCode = 0;
	
	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == INVALID_SOCKET)
    {
		SET_RED
		printf("ֱ��ʱsocket ʧ��[%d]\n", GetLastError());
		SET_BACK

		return SOCKET_ERROR;
    }
	
	addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr(param.DstIP);
    addrServer.sin_port = htons(param.DstPort);
	
	if(connect(sock,(const struct sockaddr *)&addrServer,sizeof(sockaddr)) != 0)
    {
		SET_RED
        printf("ֱ��ʱconnect ʧ��[%d]\n", GetLastError());
		SET_BACK
        return SOCKET_ERROR;
    }

	return sock;
}

//�ɹ�Ϊ0��ʧ��Ϊ<0, �ļ�Ϊ��>0
int getAgentParam(tagNewQLGCProxy_Certify &pro, PARAM &param)
{
#define			readParamNum	8
#define			GCC_PROXY_HEAD	"CDYFSUPP"
#define			MAX_PARAM_LEN		256
	
	char		res[256];

	FILE	*fp = fopen(param.AgentParamName, "r");
	if(fp == NULL){
		printf("open[%s]����", param.AgentParamName);
		return -1;
	}
	memset(&pro, 0, sizeof(pro));
	memcpy(&pro.strMagic, GCC_PROXY_HEAD, strlen(GCC_PROXY_HEAD));
	
	for(int i = 0; i < readParamNum; ++i)
	{
		memset(res, 0, sizeof(res));
		if(fgets(res, sizeof(res)-1, fp) == NULL)
		{
			printf("���ļ�[%s]�г���\n", param.AgentParamName);
			return 1;
		}
		//��β��\nȥ��
		res[strlen(res)-1]=0;
		switch (i)
		{
		case 0:
			pro.ucType = atoi(res);
			break;
		case 1:
			pro.bAuth = atoi(res);
			break;
		case 2:
			strncpy(pro.strUserName, res, sizeof(pro.strUserName)-1);
			break;
		case 3:
			strncpy(pro.strUserPassword, res, sizeof(pro.strUserPassword)-1);
			break;
		case 4:
			strncpy(pro.strAgentAddress, res, sizeof(pro.strAgentAddress)-1);
			break;
		case 5:
			pro.usAgentPort = atoi(res);
			break;
		case 6:
			strncpy(pro.strIPAddress, res, sizeof(pro.strIPAddress)-1);
			break;
		case 7:
			pro.usPort = atoi(res);
			break;
		}
	}

	printf("������֤����[%d]\n", pro.ucType);
	printf("�Ƿ�Ҫ��֤[%d]\n", pro.bAuth);
	printf("��֤���û���[%s]\n", pro.strUserName);
	printf("��֤������[%s]\n", pro.strUserPassword);
	printf("���������IP(IP��������)[%s]\n", pro.strAgentAddress);
	printf("����������Ķ˿�[%d]\n", pro.usAgentPort);
	printf("Ŀ�������IP(IP��������)[%s]\n", pro.strIPAddress);
	printf("Ŀ��������˿�[%d]\n", pro.usPort);

	return 0;
}

SOCKET doGcSocket(PARAM &param, cls_proxy_green& gree)
{
	int								err, port;
	int								greeSocket;
	tagNewQLGCProxy_Certify			pro;//��Э��ṹ
	
	

	err = gree.instance();
	if( err == 0 )
	{
		SET_RED
		printf("ʹ����ɫͨ��ʱ�������׽ӿڴ���\n");
		SET_BACK
		return SOCKET_ERROR;
	}
	port = err;
	
	//�����ɫͨ����IP��ַ
	tagQLGCProxy_IPPortIn			proxy_addr;
	memset(&proxy_addr, 0, sizeof(proxy_addr));
	strncpy(proxy_addr.szIP, param.GcIP,sizeof(param.GcIP));
	proxy_addr.usPort = param.GcPort;
	gree.comcmd( CT_SET_IP_PORT, (void*)&proxy_addr, NULL );

	err = getAgentParam(pro, param);
	if(err > 0)//����Э��
	{
		printf("����Э��\n");
		err = proxy_green("127.0.0.1", port, param.DstIP, param.DstPort, &greeSocket);
	}else if(err == 0)//����Э��
	{
		err = proxy_green_new("127.0.0.1", port, pro, &greeSocket);
		printf("����Э��\n");
	}else{
		printf("��ȡ�����ļ�����\n");
		return SOCKET_ERROR;
	}
	
	if(err != 0)
	{
		SET_RED
		printf("������ɫͨ�����׽��ֳ���,ErrorCode[%d]\n����:\n\t�Ƿ���GCClient.dll, GC�Ƿ�����,GCSIP,GCSPORT \n", err);
		SET_BACK
		return SOCKET_ERROR;
	}	

	return greeSocket;
}



int doTest(PARAM &param, int index)
{
	SOCKET							sock;
	int								errCode = 0;
	cls_proxy_green					gree;
	unsigned int					recvTimeOut = 5000;//��ʱ��Ϊ5��

	switch(param.useGc)
	{
	case 0:
		printf("��[%d]�β��ԣ���ͨ����ɫͨ��\n", index);
		sock = doDirSocket(param);
		if(sock == SOCKET_ERROR)
			return -1;
		break;
	case 1:
		printf("��[%d]�β��ԣ�Ҫͨ����ɫͨ��\n", index);
		sock = doGcSocket(param, gree);
		if(sock == SOCKET_ERROR)
			return -2;
		break;
	default:
		errCode = -3;
	}

// 	if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&recvTimeOut, sizeof(unsigned int)) != 0)
// 	{
// 		SET_RED
// 		printf("�����׽��ֳ�ʱʱ�����\n");
// 		SET_BACK
// 		errCode = -4;
// 	}
// 
// 	if(errCode < 0)
// 		return errCode;

	errCode = _doTest(param, sock, index);
	
	closesocket(sock);

	return errCode;
}

int main(int argc, char *argv[])
{
	PARAM					param;
	int						errCode;
	time_t					begin, end;

	if(argc != 11)
	{
		printf("usage: DstIP DStPort GcSIP GcSPort useGc isEcho repeatTimes maxFrameSize testFileName\n");
		return -1;
	}
	
	if(Init() != 0)
		return -2;

	if(fillParam(param, argc-1, argv+1) != 0)
		return -3;

	printParam(param);
	printf("���ڲ��Խ����\n\t����ǻ��ԣ�ֻ����Ļ�ϴ�ӡ�����Ƿ�ɹ�\n\t�ǻ��ԣ�������Ļ�ϴ�ӡ�����յ������ݣ�����ӡ�յ����ֽ���\n\n\n");
	printf("\n\n��ʼ�����ϲ������в���\n");

	time(&begin);
	for(int i = 1; i <= param.repeatTime; ++i)
	{	
		errCode = doTest(param, i);
		if(errCode == 0)
		{
			printf("��[%d]�β��Խ����SUCCESS\n", i);
		}else
		{
			SET_RED
			printf("��[%d]�β��Խ����FAILED\n", i);
			SET_BACK
			break;
		}
	}
	time(&end);
	printf("���Գ�������[%d]��\n",end - begin);

	if(errCode == 0)
	{
		printf("���ղ��Խ����SUCCESS\n");
	}else
	{
		SET_RED
		printf("���ղ��Խ����FAILED\n");
		SET_BACK
	}
	
	return 0;
}