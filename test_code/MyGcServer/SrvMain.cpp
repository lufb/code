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
		//1 收数据	
		memset(recvBuffer, '\0', sizeof(recvBuffer));
		recvSize = ::recv(cliSock, recvBuffer, MAX_FRAME, 0);
		if ( recvSize <= 0)         
		{   
			printf("SRV:CLOSE$$$$$$$$$$$$$$$$$$$$$$$$$$[%d]\n",GetLastError());
			//OnLikClose();
			break;         
		}
		//2 判断收到的数据，进行相应处理
		if( ( nRes = MHttp::GetType(recvBuffer, recvSize) ) < 0)
		{
			printf("SRV:解析头发生错误[%d]\n", nRes);
			break;
		}
		switch(nRes)
		{
			case MHttp::TP_HEART:
				printf("SRV:收到HTTP心跳\n");//不会再有这个东西的了
				nRes = dealHeart(recvBuffer, MAX_FRAME, recvSize, param);
				break;
			case MHttp::TP_CHANNEL:
				printf("SRV:收到TP_CHANNEL\n");	
				nRes = dealChannel(recvBuffer, MAX_FRAME, recvSize, cliSock);	
				break;
			case MHttp::TP_UNIT:
				printf("SRV:收到TP_UNIT\n");
				nRes = dealUnit(recvBuffer, MAX_FRAME, recvSize, param);
				break;
			case MHttp::TP_TRADEUNIT:
				printf("SRV:收到TP_TRADEUNIT\n");
				nRes = dealTradeUnit(recvBuffer, MAX_FRAME, recvSize, param);
				break;
			case MHttp::TP_DATA:
				printf("SRV:收到TP_DATA\n");
				nRes = dealData(recvBuffer, MAX_FRAME, recvSize, param);
				break;
			case MHttp::TP_CLOSE:
				printf("SRV:收到TP_CLOSE***********************************************\n");
				//nRes = dealLinkClose();//这儿应该不用去获取相应的数据，直接把该链路关掉
				//goto End;
				//continue;
				nRes = 0;
				break;
			default:
				printf("SRV:收到未知HTTP协议[%d]", nRes);
				nRes = -1;
				break;
		}
		if(nRes < 0)//出错
		{
			break;//跳出while循环，释放资源
		}
		//else//继续收数据
	}
End:
	//关闭套接字资源，内存暂不用考虑去释放
	if(param->cliSocket != INVALID_SOCKET)
	{
		printf("Srv:关闭向下套接字\n");
		closesocket(param->cliSocket);
		param->cliSocket = INVALID_SOCKET;
	}
	if(param->srvSocket != INVALID_SOCKET)
	{
		printf("Srv:关闭向上套接字\n");
		closesocket(param->srvSocket);
		param->srvSocket = INVALID_SOCKET;
	}
//为了测试，不让线程退出
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
		printf("第一个字节不为#，非法了\n");		
		return -1;
	}

	if(param->tradeState == TS_AUTH)
	{
		IPOS_HEAD* pHead			= (IPOS_HEAD*)tmpBuffer;
		CU_CHECKREPLY* pCheckReply	= (CU_CHECKREPLY*)(tmpBuffer+sizeof(IPOS_HEAD));

		//note:这儿？？？？？？？？？
		if( param->hadUsed < (sizeof(IPOS_HEAD)+sizeof(UC_CHECK)) )
		{
			printf("委托认证包还没收全\n");//test point
			return -1; //继续等待
		}

		param->hadUsed -= (sizeof(IPOS_HEAD)+sizeof(UC_CHECK));
		if(param->hadUsed > 0)
		{
			printf("fuck 收到的数据大了一点\n");
			memmove(param->tradeBuffr, param->tradeBuffr+sizeof(IPOS_HEAD)+sizeof(UC_CHECK), param->hadUsed);
		}else{
			;//printf("fuck 收到的数据刚好那么多了嘛\n");//test point
		}

		pHead->Flag		= '#';          //IP数据包起始标志'#',SerialPort数据包起始标志'0xFF'
		pHead->Ver		= IPOSVER;			//版本号
		pHead->sendDataLength = 0;      //编码之后数据头后的发送数据包长度
		pHead->Type = TYPE_CU_CHECKREPLY;                   //数据包类型
		pHead->DataLength = sizeof(CU_CHECKREPLY);
		pHead->Chk = 0;
		
		pCheckReply->Reply	= CERTIFY_SUCCESS;
		pCheckReply->Chk	= 0;

		if((nRes = myHttpSend(param->cliSocket, tmpBuffer, sizeof(IPOS_HEAD)+sizeof(CU_CHECKREPLY), param)) != sizeof(IPOS_HEAD)+sizeof(CU_CHECKREPLY))
		{
			printf("CLI:Data::向客户端回数据失败[%d]\n", nRes);
			return -4;
		}

		param->tradeState = TS_CONNECT;	//置状态
	}else if(param->tradeState == TS_CONNECT)
	{
		if( param->hadUsed < (sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO)) )
		{
			printf("委托连接包还没收全\n");
			return -1; //继续等待
		}

		param->hadUsed -= (sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO));
		if( param->hadUsed > 0 )
		{
			printf("送的连接包里面带有其它数据\n");
			memmove(param->tradeBuffr, param->tradeBuffr+(sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO)), param->hadUsed);
		}else{

		}

		IPOS_HEAD* pHead 	= (IPOS_HEAD*)tmpBuffer;
		CU_CHECKREPLY* pConnectReply	= (CU_CHECKREPLY*)(tmpBuffer+sizeof(IPOS_HEAD));
		pHead->Flag		= '#';          //IP数据包起始标志'#',SerialPort数据包起始标志'0xFF'
		pHead->Ver		= IPOSVER;			//版本号
		pHead->sendDataLength = 0;      //编码之后数据头后的发送数据包长度
		pHead->Type = TYPE_CU_CONNECTREPLY;                   //数据包类型
		pHead->DataLength = sizeof(CU_CHECKREPLY);
		pHead->Chk = 0;
		
		pConnectReply->Reply	= CONNECT_SUCCESS;
		pConnectReply->Chk		= 0;

		if((nRes = myHttpSend(param->cliSocket, tmpBuffer, sizeof(IPOS_HEAD)+sizeof(CU_CHECKREPLY), param)) != sizeof(IPOS_HEAD)+sizeof(CU_CHECKREPLY))
		{
			printf("CLI:Data::向客户端回CONN数据失败[%d]\n", nRes);
			return -4;
		}

		pHead	= (IPOS_HEAD*)tmpBuffer;
		UC_CLIENTINFO* pClientInfo	= (UC_CLIENTINFO*)(tmpBuffer+sizeof(IPOS_HEAD));
		pHead->Flag		= '#';          //IP数据包起始标志'#',SerialPort数据包起始标志'0xFF'
		pHead->Ver		= IPOSVER;			//版本号
		pHead->sendDataLength = 0;      //编码之后数据头后的发送数据包长度
		pHead->Type = TYPE_UC_CLIENTINFO;                   //数据包类型
		pHead->DataLength = sizeof(UC_CLIENTINFO);
		pHead->Chk = 0;
		
		pClientInfo->IpAddr.s_addr = inet_addr(param->cliIP);
		pClientInfo->Port = htons(param->cliPort);
		pClientInfo->Chk		= 0;

		if((nRes = mySend(param->srvSocket, tmpBuffer, sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO))) != sizeof(IPOS_HEAD)+sizeof(UC_CLIENTINFO))
		{
			printf("Trade[%d][%s]\n", strlen(tmpBuffer), tmpBuffer);
			printf("CLI:Data::向服务器发Conn数据失败[%d]\n", nRes);
			return -4;
		}

		if(param->hadUsed > 0)
		{
			printf("有遗留数据\n");
			if((nRes = mySend(param->srvSocket, param->tradeBuffr, param->hadUsed)) != param->hadUsed)
			{
				printf("CLI:Data::向服务器发尾部数据失败[%d]\n", nRes);
				return -4;
			}
		}else
		{
			printf("没得遗留数据\n");
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
		printf("SRV:GetData提取的数据错误\n");	
		return -2;
	}

	if(param->srvSocket == INVALID_SOCKET)
	{
		printf("SRV:还没建立单元信息\n");
		return -3;
	}

	if(param->isTrade && param->tradeState != TS_DONE)
	{
		nRes = dealTradeData(recvBuffer+nDataStart, nDataSize, param);
		if(nRes != 0)
		{
			printf("处理委托部份数据出错[%d]\n", nRes);
			return -5;
		}

		return 0;
	}

	printf("开始发普通数据\n");
	
	if((nRes = mySend(param->srvSocket, recvBuffer + nDataStart, nDataSize)) != nDataSize)
	{
		printf("SRV:Data::发送数据失败[%d]\n", nRes);
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
		printf("%d:socket 失败\n",GetLastError());
		return -1;
    }
	
	addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr(srvIP);
    addrServer.sin_port = htons(srvPort);
	
	if(connect(sock,(const struct sockaddr *)&addrServer,sizeof(sockaddr)) != 0)
    {
        printf("SRV::connect 失败[%d]\n", GetLastError());
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
			//无效的域名
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
		//本来就是IP地址
		strncpy(dst, strDns, dstSize);
	}

	return 0;
}
int dealUnit(char *recvBuffer, size_t bufferSize, size_t recvSize, THREAD_PARAM *param)
{
	char					sendBuffer[MAX_FRAME] = {'\0'};
	unsigned short			usUnitSerial;
	unsigned long			ulLinkNo;
	char					szIPSource[255]	= {'\0'};	//域名解析时用到的一个临时变量
	char					szIP[255]	= {'\0'};		//中途的HTTP代理使用
	unsigned short			usPort		= 0;
	int						nErrorCode = 1;
	int						nRes;

	if( ( nRes = MHttp::GetUnit(recvBuffer, recvSize, usUnitSerial, ulLinkNo, szIPSource, usPort, nErrorCode) ) < 0)
	{
		printf("SRV:Unit::GetUnit Error[%d]\n", nRes);	
		return -1;
	}

	//存储GCLNKNO与序号（GCLNKNO与序号在我们向客户端回复数据时，组包时要用到这两个字段）
	param->ulLinkNo = ulLinkNo;
	param->usUnitSerial = usUnitSerial;

	//域名转换
	DNSToIP(szIPSource, sizeof(szIPSource), szIP, sizeof(szIP)-1);

	//这儿是阻塞的
	if((nRes = myConnect(param->srvSocket, szIP, usPort)) < 0)
	{
		printf("SRV:Unit::connect[%s:%d]Error[%d]\n", szIP, usPort, nRes);
		return -2;
	}

	//设置状态为已连接
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
	char							szIPSource[255]	= {'\0'};	//域名解析时用到的一个临时变量
	char							szIP[255]	= {'\0'};		//中途的HTTP代理使用
	unsigned short					usPort		= 0;
	int								nErrorCode = 1;
	int								nRes;
	char							sendBuffer[MAX_FRAME] = {'\0'};

	if( ( nRes = MHttp::GetTradeUnit(recvBuffer, recvSize, usUnitSerial, ulLinkNo, szIPSource, usPort, nErrorCode) ) < 0)
	{
		printf("Srv:GetTradeUnit Error[%d]\n", nRes);	
		return -1;
	}

	//存储GCLNKNO与序号（GCLNKNO与序号在我们向客户端回复数据时，组包时要用到这两个字段）
	param->ulLinkNo = ulLinkNo;
	param->usUnitSerial = usUnitSerial;

	//域名转换
	DNSToIP(szIPSource, sizeof(szIPSource), szIP, sizeof(szIP)-1);

	//这儿是阻塞的
	if((nRes = myConnect(param->srvSocket, szIP, usPort)) < 0)
	{
		printf("SRV:Unit::connect[%s:%d]Error[%d]\n", szIP, usPort, nRes);
		return -2;
	}

	//设置为委拖，并设置委托状态为初始化状态
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

	//直接回复一个11字节的心跳包
	if( ( nRes = MHttp::BuildData(sendBuffer,MAX_FRAME, param->usUnitSerial, param->ulLinkNo, sendBuffer, 11)) < 0 )
	{
		printf("Srv:组心跳包错误[%d]\n", nRes);
		return -1;
	}

	//向客户端转发数据
	if((nRes = ::send(param->cliSocket, sendBuffer, nRes, 0)) != nRes)
	{
		printf("Srv:向客户端回复心跳包出错[%d]\n", GetLastError());
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
		printf("SRV:获取连接信息出错\n");	
		return -1;
	}
	memset(sendBuffer, '\0', sizeof(sendBuffer));
	//将解析出来的包中的相关信息返回给客户端
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


