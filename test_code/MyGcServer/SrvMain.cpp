#include "StdAfx.h"
#include "SrvMain.h"
#include "Base.h"
#include <assert.h>
#include "MHttp.h"

DWORD WINAPI SrvThread(LPVOID lpParam)
{
	char				recvBuffer[MAX_FRAME];
	THREAD_PARAM		*param = (THREAD_PARAM*)lpParam;
	assert(param != NULL);
	int					cliSock = param->cliSocket;
	assert(cliSock != INVALID_SOCKET);
	int					recvSize;
	int					nRes;
	
	while(1)
	{
		//1 ������	
		memset(recvBuffer, '\0', sizeof(recvBuffer));
		recvSize = ::recv(cliSock, recvBuffer, MAX_FRAME, 0);
		if ( recvSize <= 0)         
		{   
			printf("SRV:CLOSE$$$$$$$$$$$$$$$$$$$$$$$$$$[%d]\n",GetLastError());
			//OnLikClose();
			break;         
		}
		//2 �ж��յ������ݣ�������Ӧ����
		if( ( nRes = MHttp::GetType(recvBuffer, recvSize) ) < 0)
		{
			printf("SRV:����ͷ��������[%d]\n", nRes);
			break;
		}
		switch(nRes)
		{
			case MHttp::TP_HEART:
				printf("SRV:�յ�HTTP����\n");//�������������������
				nRes = dealHeart(recvBuffer, MAX_FRAME, recvSize, param);
				break;
			case MHttp::TP_CHANNEL:
				printf("SRV:�յ�TP_CHANNEL\n");	
				nRes = dealChannel(recvBuffer, MAX_FRAME, recvSize, cliSock);	
				break;
			case MHttp::TP_UNIT:
				printf("SRV:�յ�TP_UNIT\n");
				nRes = dealUnit(recvBuffer, MAX_FRAME, recvSize, param);
				break;
			case MHttp::TP_TRADEUNIT:
				printf("SRV:�յ�TP_TRADEUNIT\n");
				nRes = dealTradeUnit(recvBuffer, MAX_FRAME, recvSize, param);
				break;
			case MHttp::TP_DATA:
				printf("SRV:�յ�TP_DATA\n");
				nRes = dealData(recvBuffer, MAX_FRAME, recvSize, param);
				break;
			case MHttp::TP_CLOSE:
				printf("SRV:�յ�TP_CLOSE***********************************************\n");
				//nRes = dealLinkClose();//���Ӧ�ò���ȥ��ȡ��Ӧ�����ݣ�ֱ�ӰѸ���·�ص�
				//goto End;
				//continue;
				nRes = 0;
				break;
			default:
				printf("SRV:�յ�δ֪HTTPЭ��[%d]", nRes);
				nRes = -1;
				break;
		}
		if(nRes < 0)//����
		{
			break;//����whileѭ�����ͷ���Դ
		}
		//else//����������
	}
End:
	//�ر��׽�����Դ���ڴ��ݲ��ÿ���ȥ�ͷ�
	if(param->cliSocket != INVALID_SOCKET)
	{
		printf("Srv:�ر������׽���\n");
		closesocket(param->cliSocket);
		param->cliSocket = INVALID_SOCKET;
	}
	if(param->srvSocket != INVALID_SOCKET)
	{
		printf("Srv:�ر������׽���\n");
		closesocket(param->srvSocket);
		param->srvSocket = INVALID_SOCKET;
	}
//Ϊ�˲��ԣ������߳��˳�
	while(1)
	{
		Sleep(500);
	}

	return 0;
}


int dealTradeData(char *recvBuffer, size_t bufferSize, THREAD_PARAM *param)
{


	char						tmpBuffer[1024] = {'\0'};
	int							nRes;

	memcpy(param->tradeBuffr, recvBuffer, bufferSize);
	param->hadUsed += bufferSize;

	if(param->tradeBuffr[0] != '#')
	{
		printf("��һ���ֽڲ�Ϊ#���Ƿ���\n");		
		return -1;
	}

	if(param->tradeState == TS_AUTH)
	{
		IPOS_HEAD* pHead			= (IPOS_HEAD*)tmpBuffer;
		CU_CHECKREPLY* pCheckReply	= (CU_CHECKREPLY*)(tmpBuffer+sizeof(IPOS_HEAD));

		//note:���������������������
		if( param->hadUsed < (sizeof(IPOS_HEAD)+sizeof(UC_CHECK)) )
		{
			printf("ί����֤����û��ȫ\n");//test point
			return -1; //�����ȴ�
		}

		param->hadUsed -= (sizeof(IPOS_HEAD)+sizeof(UC_CHECK));
		if(param->hadUsed > 0)
		{
			printf("fuck �յ������ݴ���һ��\n");
			memmove(param->tradeBuffr, param->tradeBuffr+sizeof(IPOS_HEAD)+sizeof(UC_CHECK), param->hadUsed);
		}else{
			;//printf("fuck �յ������ݸպ���ô������\n");//test point
		}

		pHead->Flag		= '#';          //IP���ݰ���ʼ��־'#',SerialPort���ݰ���ʼ��־'0xFF'
		pHead->Ver		= IPOSVER;			//�汾��
		pHead->sendDataLength = 0;      //����֮������ͷ��ķ������ݰ�����
		pHead->Type = TYPE_CU_CHECKREPLY;                   //���ݰ�����
		pHead->DataLength = sizeof(CU_CHECKREPLY);
		pHead->Chk = 0;
		
		pCheckReply->Reply	= CERTIFY_SUCCESS;
		pCheckReply->Chk	= 0;

		if((nRes = myHttpSend(param->cliSocket, tmpBuffer, sizeof(IPOS_HEAD)+sizeof(CU_CHECKREPLY), param)) != sizeof(IPOS_HEAD)+sizeof(CU_CHECKREPLY))
		{
			printf("CLI:Data::��ͻ��˻�����ʧ��[%d]\n", nRes);
			return -4;
		}

		param->tradeState = TS_CONNECT;	//��״̬
	}else if(param->tradeState == TS_CONNECT)
	{
		if( param->hadUsed < (sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO)) )
		{
			printf("ί�����Ӱ���û��ȫ\n");
			return -1; //�����ȴ�
		}

		param->hadUsed -= (sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO));
		if( param->hadUsed > 0 )
		{
			printf("�͵����Ӱ����������������\n");
			memmove(param->tradeBuffr, param->tradeBuffr+(sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO)), param->hadUsed);
		}else{

		}

		IPOS_HEAD* pHead 	= (IPOS_HEAD*)tmpBuffer;
		CU_CHECKREPLY* pConnectReply	= (CU_CHECKREPLY*)(tmpBuffer+sizeof(IPOS_HEAD));
		pHead->Flag		= '#';          //IP���ݰ���ʼ��־'#',SerialPort���ݰ���ʼ��־'0xFF'
		pHead->Ver		= IPOSVER;			//�汾��
		pHead->sendDataLength = 0;      //����֮������ͷ��ķ������ݰ�����
		pHead->Type = TYPE_CU_CONNECTREPLY;                   //���ݰ�����
		pHead->DataLength = sizeof(CU_CHECKREPLY);
		pHead->Chk = 0;
		
		pConnectReply->Reply	= CONNECT_SUCCESS;
		pConnectReply->Chk		= 0;

		if((nRes = myHttpSend(param->cliSocket, tmpBuffer, sizeof(IPOS_HEAD)+sizeof(CU_CHECKREPLY), param)) != sizeof(IPOS_HEAD)+sizeof(CU_CHECKREPLY))
		{
			printf("CLI:Data::��ͻ��˻�CONN����ʧ��[%d]\n", nRes);
			return -4;
		}

		pHead	= (IPOS_HEAD*)tmpBuffer;
		UC_CLIENTINFO* pClientInfo	= (UC_CLIENTINFO*)(tmpBuffer+sizeof(IPOS_HEAD));
		pHead->Flag		= '#';          //IP���ݰ���ʼ��־'#',SerialPort���ݰ���ʼ��־'0xFF'
		pHead->Ver		= IPOSVER;			//�汾��
		pHead->sendDataLength = 0;      //����֮������ͷ��ķ������ݰ�����
		pHead->Type = TYPE_UC_CLIENTINFO;                   //���ݰ�����
		pHead->DataLength = sizeof(UC_CLIENTINFO);
		pHead->Chk = 0;
		
		pClientInfo->IpAddr.s_addr = inet_addr(param->cliIP);
		pClientInfo->Port = htons(param->cliPort);
		pClientInfo->Chk		= 0;

		if((nRes = mySend(param->srvSocket, tmpBuffer, sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO))) != sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO))
		{
			printf("Trade[%d][%s]\n", strlen(tmpBuffer), tmpBuffer);
			printf("CLI:Data::���������Conn����ʧ��[%d]\n", nRes);
			return -4;
		}

		if(param->hadUsed > 0)
		{
			printf("����������\n");
			if((nRes = mySend(param->srvSocket, param->tradeBuffr, param->hadUsed)) != param->hadUsed)
			{
				printf("CLI:Data::���������β������ʧ��[%d]\n", nRes);
				return -4;
			}
		}else
		{
			printf("û����������\n");
		}

		param->tradeState  = TS_DONE;
		param->isConnected = 1;

	}else{
		assert(0);
		return -1;
	}

	return 0;
}

int dealData(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM *param)
{
	int							nRes;
	unsigned short				usUnitSerial;
	unsigned long				ulLinkNo;
	int							nDataStart, nDataSize;

	if( ( nRes = MHttp::GetData(recvBuffer, recvSize, usUnitSerial, ulLinkNo, nDataStart, nDataSize) ) < 0)
	{
		printf("SRV:GetData Error[%d]\n", nRes);	
		return -1;
	}

	if( nDataStart > recvSize ||  nDataSize > (recvSize-nDataStart))
	{
		printf("SRV:GetData��ȡ�����ݴ���\n");	
		return -2;
	}

	if(param->srvSocket == INVALID_SOCKET)
	{
		printf("SRV:��û������Ԫ��Ϣ\n");
		return -3;
	}

	if(param->isTrade && param->tradeState != TS_DONE)
	{
		nRes = dealTradeData(recvBuffer+nDataStart, nDataSize, param);
		if(nRes != 0)
		{
			printf("����ί�в������ݳ���[%d]\n", nRes);
			return -5;
		}

		return 0;
	}

	printf("��ʼ����ͨ����\n");
	
	if((nRes = mySend(param->srvSocket, recvBuffer + nDataStart, nDataSize)) != nDataSize)
	{
		printf("SRV:Data::��������ʧ��[%d]\n", nRes);
		return -4;
	}

	return 0;
}

int myConnect(SOCKET &srvSock, char* srvIP, short srvPort)
{
	SOCKET						sock;
	struct	sockaddr_in			addrServer;

	sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == INVALID_SOCKET)
    {
		printf("%d:socket ʧ��\n",GetLastError());
		return -1;
    }
	
	addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr(srvIP);
    addrServer.sin_port = htons(srvPort);
	
	if(connect(sock,(const struct sockaddr *)&addrServer,sizeof(sockaddr)) != 0)
    {
        printf("SRV::connect ʧ��[%d]\n", GetLastError());
        return -2;
    }
	srvSock = sock;

	return 0;
}

// MString MSocket::ULToIP(unsigned long lIn)
// {
// 	struct in_addr			sttempaddr;
// 	
// 	sttempaddr.s_addr = lIn;
// 	return(inet_ntoa(sttempaddr));
// }

int DNSToIP(char* strDns, size_t size, char* dst, size_t dstSize)
{
	assert(strDns != NULL && size > 0);

	struct hostent				*	lpstHost;
	sockaddr_in                     stAddr;
	char						tmpBuffer[256] = {'\0'};
	
	if ( inet_addr(strDns) == INADDR_NONE )
	{
		if ( (lpstHost = gethostbyname(strDns)) == NULL )
		{
			//��Ч������
			dst[0] = '\0';
			return -1;
		}
		
		memcpy((char *)&stAddr.sin_addr,(char *)lpstHost->h_addr,lpstHost->h_length);
		
		struct in_addr			sttempaddr;
		sttempaddr.s_addr = stAddr.sin_addr.s_addr;
		strncpy(dst, inet_ntoa(sttempaddr), dstSize);
		
	}
	else
	{
		//��������IP��ַ
		strncpy(dst, strDns, dstSize);
	}

	return 0;
}
int dealUnit(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM *param)
{
	char					sendBuffer[MAX_FRAME] = {'\0'};
	unsigned short			usUnitSerial;
	unsigned long			ulLinkNo;
	char					szIPSource[255]	= {'\0'};	//��������ʱ�õ���һ����ʱ����
	char					szIP[255]	= {'\0'};		//��;��HTTP����ʹ��
	unsigned short			usPort		= 0;
	int						nErrorCode = 1;
	int						nRes;

	if( ( nRes = MHttp::GetUnit(recvBuffer, recvSize, usUnitSerial, ulLinkNo, szIPSource, usPort, nErrorCode) ) < 0)
	{
		printf("SRV:Unit::GetUnit Error[%d]\n", nRes);	
		return -1;
	}

	//�洢GCLNKNO����ţ�GCLNKNO�������������ͻ��˻ظ�����ʱ�����ʱҪ�õ��������ֶΣ�
	param->ulLinkNo = ulLinkNo;
	param->usUnitSerial = usUnitSerial;

	//����ת��
	DNSToIP(szIPSource, sizeof(szIPSource), szIP, sizeof(szIP)-1);

	//�����������
	if((nRes = myConnect(param->srvSocket, szIP, usPort)) < 0)
	{
		printf("SRV:Unit::connect[%s:%d]Error[%d]\n", szIP, usPort, nRes);
		return -2;
	}

	//����״̬Ϊ������
	assert(param->isConnected == 0);
	param->isConnected = 1;

	if( (nRes = MHttp::BuildUnit(sendBuffer, MAX_FRAME, usUnitSerial, ulLinkNo, szIP, usPort, nRes)) < 0 )
	{
		return -3;
	}


	if((nErrorCode = mySend(param->cliSocket, sendBuffer, nRes)) != nRes)
	{
		printf("SRV:TP_CHANNEL::SendError[%d]\n", nErrorCode);
		return -3;
	}

	return 0;
}

int dealTradeUnit(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM *param)
{
	unsigned short					usUnitSerial;
	unsigned long					ulLinkNo;
	char							szIPSource[255]	= {'\0'};	//��������ʱ�õ���һ����ʱ����
	char							szIP[255]	= {'\0'};		//��;��HTTP����ʹ��
	unsigned short					usPort		= 0;
	int								nErrorCode = 1;
	int								nRes;
	char							sendBuffer[MAX_FRAME] = {'\0'};

	if( ( nRes = MHttp::GetTradeUnit(recvBuffer, recvSize, usUnitSerial, ulLinkNo, szIPSource, usPort, nErrorCode) ) < 0)
	{
		printf("Srv:GetTradeUnit Error[%d]\n", nRes);	
		return -1;
	}

	//�洢GCLNKNO����ţ�GCLNKNO�������������ͻ��˻ظ�����ʱ�����ʱҪ�õ��������ֶΣ�
	param->ulLinkNo = ulLinkNo;
	param->usUnitSerial = usUnitSerial;

	//����ת��
	DNSToIP(szIPSource, sizeof(szIPSource), szIP, sizeof(szIP)-1);

	//�����������
	if((nRes = myConnect(param->srvSocket, szIP, usPort)) < 0)
	{
		printf("SRV:Unit::connect[%s:%d]Error[%d]\n", szIP, usPort, nRes);
		return -2;
	}

	//����Ϊί�ϣ�������ί��״̬Ϊ��ʼ��״̬
	assert(param->isConnected == 0);
	param->isTrade = true;
	param->tradeState = TS_AUTH;
	//param->isConnected = 1;

	if( (nRes = MHttp::BuildTradeUnit(sendBuffer, MAX_FRAME, usUnitSerial, ulLinkNo, szIP, usPort, nErrorCode)) < 0 )
	{
		printf("Srv:BuildTradeUnit Error[%d]\n", nRes);
		return -3;
	}

	if((nErrorCode = mySend(param->cliSocket, sendBuffer, nRes)) != nRes)
	{
		printf("SRV:TP_CHANNEL::SendError[%d]\n", nErrorCode);
		return -3;
	}

	return 0;
}

int dealHeart(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM* param)
{
	assert(param != 0);
	char			sendBuffer[MAX_FRAME] = {0};
	int				nRes;
	sendBuffer[0] = '#';

	//ֱ�ӻظ�һ��11�ֽڵ�������
	if( ( nRes = MHttp::BuildData(sendBuffer,MAX_FRAME, param->usUnitSerial, param->ulLinkNo, sendBuffer, 11)) < 0 )
	{
		printf("Srv:������������[%d]\n", nRes);
		return -1;
	}

	//��ͻ���ת������
	if((nRes = ::send(param->cliSocket, sendBuffer, nRes, 0)) != nRes)
	{
		printf("Srv:��ͻ��˻ظ�����������[%d]\n", GetLastError());
		return -2;
	}

	return 0;
}
int dealChannel(char *recvBuffer, size_t bufferSize, size_t recvSize, int cliSock)
{
	short			sSrvChanSerial;
	unsigned short	ulChannelSerial;
	int				nErrorCode = 1;
	char			sendBuffer[MAX_FRAME];
	int				nRes;
	int				errCode;

	if( ( nRes = MHttp::GetChannel(recvBuffer, recvSize, sSrvChanSerial, ulChannelSerial, nErrorCode) ) < 0)
	{
		printf("SRV:��ȡ������Ϣ����\n");	
		return -1;
	}
	memset(sendBuffer, '\0', sizeof(sendBuffer));
	//�����������İ��е������Ϣ���ظ��ͻ���
	if( (nRes = MHttp::BuildChannel(sendBuffer, MAX_FRAME, sSrvChanSerial, ulChannelSerial, nErrorCode)) < 0 )
	{
		printf("SRV:TP_CHANNEL:BuildChannel Error[%d]", nRes);
		return -2;
	}
	if((errCode = mySend(cliSock, sendBuffer, nRes)) != nRes)
	{
		printf("SRV:TP_CHANNEL::SendError[%d]\n", errCode);
		return -3;
	}

	return 0;
}


