#include <stdio.h>
#include "main.h"
#include "MthreadMgr.h"
#include "global.h"
#include "global.h"
#include "Mhttp.h"
#include "gcCliInterface.h"



unsigned short workThreadCount = 0;
HANDLE	m_hWorkHand[MAX_WORK_THREAD];

MThreadMgr::MThreadMgr()
{
	m_hLisHand = NULL;
	initLisThrPar();
}

MThreadMgr::~MThreadMgr()
{

}

int	MThreadMgr::fillLisThrPar(unsigned short lisPort, SOCKET listen, char *GcSIP, unsigned short GcSPort)
{
	m_sLisParam.m_sLisPort = lisPort;
	m_sLisParam.m_sListen = listen;
	strncpy(m_sLisParam.m_cGcSIP, GcSIP, sizeof(m_sLisParam.m_cGcSIP)-1);
	m_sLisParam.m_usGcSPort = GcSPort;

	return 0;
}

void MThreadMgr::initLisThrPar()
{
	m_sLisParam.m_sLisPort = 0;
	m_sLisParam.m_sListen = INVALID_SOCKET;
	m_sLisParam.m_usGcSPort = 0;
	memset(m_sLisParam.m_cGcSIP, '\n', sizeof(m_sLisParam.m_cGcSIP));
}

int	MThreadMgr::lisThreadInit(unsigned short lisPort)
{
	SOCKET						sListen;
	struct sockaddr_in			local;


	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR)
    {
        printf("socket() failed: %d\n", GetLastError());
        return 1;
    }
	
	local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(lisPort);
	
	if (bind(sListen, (struct sockaddr *)&local, 
		sizeof(local)) == SOCKET_ERROR)
    {
        printf("bind() failed: %d\n", GetLastError());
        return 1;
    }

	m_hLisHand = (void *)_beginthreadex(NULL, 0, acceptThread, (void* )sListen, 0, NULL);

	return 0;
}

int connDst(char *dstIP, unsigned short dstPort, SOCKET	&srvSock)
{
	struct sockaddr_in				addrServer;
	
	srvSock = socket(AF_INET, SOCK_STREAM,0);
	if(srvSock == INVALID_SOCKET)
    {
		return GetLastError();
    }
	
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = inet_addr(dstIP);
	addrServer.sin_port = htons(dstPort);
	
	if(connect(srvSock,(const struct sockaddr *)&addrServer,sizeof(sockaddr)) != 0)
    {
		closesocket(srvSock);
        return GetLastError();
    }
	
	return 0;
}

int channel(SOCKET srvSock)
{	
	char						buffer[MAX_BUFFER];
	int							size;

	size = MHttp::BuildChannel(buffer, sizeof(buffer), 0, 0, 0);

	if(send(srvSock, buffer, size, 0) != size)
	{
		printf("send channel[%d]字节，出错[%d]\n", size, GetLastError());
		return GetLastError();
	}

	if((size = recv(srvSock, buffer, sizeof(buffer), 0)) <= 0)
	{
		printf("recv channel出错[%d]\n", GetLastError());
		return GetLastError();
	}

	printf("收channel[%d]字节\n", size);
	return 0;
}

int commonUnit(char *recvBuffer, SOCKET cliSock, SOCKET srvSock)
{
	char					buffer[MAX_BUFFER];
	int						rc;

	tagQLGCProxy_Certify * pcertify = (tagQLGCProxy_Certify *)recvBuffer;
	rc = MHttp::BuildUnit(buffer, MAX_BUFFER, 0, 0, pcertify->strIPAddress, pcertify->usPort, 0);

	if((rc = send(srvSock, buffer, rc, 0)) != rc)
	{
		printf("commonUnit:send Error[%d]", GetLastError());
		return -1;
	}

	if((rc = recv(srvSock, buffer, MAX_BUFFER, 0)) <= 0)
	{
		printf("commonUnit:recv Error[%d]", GetLastError());
		return -2;
	}

	memset(buffer, 0 , sizeof(buffer));

	tagQLGCProxy_Apply * pApplay = (tagQLGCProxy_Apply *)buffer;
	tagQLGCProxy_CertifyRet * pCertify = (tagQLGCProxy_CertifyRet *)(buffer + sizeof(tagQLGCProxy_Apply));
	
	memcpy(pApplay->szFlag, "QLGC", 4);
	pApplay->ucApplyType = 1;
	pApplay->usSize = sizeof(tagQLGCProxy_CertifyRet);
	pCertify->ucReply = 0;
	pCertify->CheckCode = '!';
	
	rc = sizeof(tagQLGCProxy_Apply) + sizeof(tagQLGCProxy_CertifyRet);

	printf("向下回复单元建立好\n");
	if(send(cliSock, buffer, rc, 0) != rc)
	{
		printf("commonUnit 向下回复单元建立好出错[%d]\n", GetLastError());
		return -3;
	}

	return 0;
}

int unit(SOCKET cliSock, SOCKET srvSock)
{
	char					buffer[MAX_BUFFER];
	int						rc;

	if((rc = recv(cliSock, buffer, sizeof(buffer), 0)) < 0)
	{
		printf("unit :recv error[%d]\n", GetLastError());
		return GetLastError();
	}
	
	tagQLGCProxy_Apply *pPro = (tagQLGCProxy_Apply*)buffer;

	switch(pPro->ucApplyType)
	{
	case 1:	/** comm unit*/
		rc = commonUnit(buffer+sizeof(tagQLGCProxy_Apply), cliSock, srvSock);
		break;
	case 2:	/**trade unit*/
		break;
	default:
		printf("#####unit不晓得啥子类型呢\n");
		rc = -2;
		break;
	}
	
	if(rc == 0)
		printf("对等链路已经建立好了\n");

	return rc;
}

// <= 0 出错或者关闭
int transUp(SOCKET cliSock, SOCKET srvSock)
{
	char					rcvBuffer[MAX_BUFFER];
	char					sndBuffer[MAX_BUFFER];
	int						rc;

	if((rc = recv(cliSock, rcvBuffer, sizeof(rcvBuffer), 0)) <= 0)
	{
		printf("transUp::Cli关闭[%d]\n", GetLastError());
		return -1;
	}

	printf("UP纯数据[%d]字节\n", rc);

	rc = MHttp::BuildData(sndBuffer, MAX_BUFFER, 0, 0, rcvBuffer, rc);

	if(send(srvSock, sndBuffer, rc, 0) < 0)
	{
		printf("transUp::Srv关闭[%d]\n", GetLastError());
		return -2;
	}

	return 1;
}

// <= 0 出错或者关闭
int transDown(SOCKET cliSock, SOCKET srvSock)
{
	char					rcvBuffer[MAX_BUFFER];
	int						rc;

	int						sendsize;
	unsigned short			unit;
	unsigned long			recvlinkno;
	int						nDataStart;
	
	if((rc = recv(srvSock, rcvBuffer, sizeof(rcvBuffer), 0)) <= 0)
	{
		printf("transDown::Srv关闭[%d]\n", GetLastError());
		return -1;
	}

	printf("Down:包括http包收到[%d]\n字节", rc);
	
	rc = MHttp::GetData(rcvBuffer, rc, unit, recvlinkno, nDataStart, sendsize);
	if(rc <= 0)
	{
		printf("GetData Error[%d]\n",rc);
		return -2;
	}

	printf("Down:纯数据向下转发[%d]\n字节", sendsize);
	if(send(cliSock, rcvBuffer+nDataStart, sendsize, 0) < 0)
	{
		printf("transDown::Cli关闭[%d]\n", GetLastError());
		return -2;
	}
	
	return 1;
}

int transData(SOCKET cliSock, SOCKET srvSock)
{
	fd_set					fdRead;
	int						rc;

	while(1)
	{
		FD_ZERO(&fdRead);
		FD_SET(cliSock, &fdRead);
		FD_SET(srvSock, &fdRead);

		rc= select(0, &fdRead, NULL, NULL, NULL);
		if(rc == SOCKET_ERROR)
		{
			printf("select error[%d]\n",GetLastError());
			return -1;
		}else
		{
			for(int i = 0; i < fdRead.fd_count; ++i)
			{
				if(fdRead.fd_array[i] == srvSock)
				{
					if(transDown(cliSock, srvSock) <= 0)
						return 0;
				}
				else if(fdRead.fd_array[i] == cliSock)
				{
					if(transUp(cliSock, srvSock) <= 0)
						return 0;
				}else
				{
					printf("不晓得啥子错误\n");
				}
			}
		}

	}
}

int DNSToIP(char* strDns, size_t size, char* dst, size_t dstSize)
{	
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


unsigned int __stdcall	MThreadMgr::workThread(void * in)
{
	SOCKET						cliSock = (SOCKET)in;
	SOCKET						srvSock = INVALID_SOCKET;
	char						tmp[256] = {'\n'};
	char						GcSIP[256] = {'\n'};
	unsigned short				GcSPort;
	int							rc;

	Global_Option.getGcSIPPort(tmp, sizeof(tmp), GcSPort);

	//域名转换
	DNSToIP(tmp, sizeof(tmp), GcSIP, sizeof(GcSIP)-1);

	//step 1:connect DSP
	if((rc = connDst(GcSIP, GcSPort, srvSock)) != 0)
	{
		printf("连接Dsp[%s:%d]出错[%d]\n", GcSIP, GcSPort, rc);
		goto End;
	}

	//step 2: channel
	if((rc = channel(srvSock)) != 0)
	{
		printf("######channel出错\n");
		goto End;
	}

	//step 3:	unit

	if((rc = unit(cliSock, srvSock)) != 0)
	{
		printf("######unit出错\n");
		goto End;
	}

printf("SRV[%d]CLI[%d]\n", srvSock, cliSock);
// 	if(rc= transData(cliSock, srvSock))
// 	{
// 		printf("######transData出错[%d]\n", rc);
// 		goto End;
// 	}

	return dealClient(cliSock, srvSock);





End:
	if(cliSock != INVALID_SOCKET)
		closesocket(cliSock);
	if(srvSock != INVALID_SOCKET)
		closesocket(srvSock);

	return rc;
}

unsigned int __stdcall	MThreadMgr::acceptThread(void * in)
{
	SOCKET						sListen, sCli;
	struct sockaddr_in			client;
	int							iAddrSize = sizeof(client);

	if(NULL == in)
		return 1;
	sListen = (SOCKET)in;

	printf("开始监听\n");
	listen( sListen, 5);
	
	while(1)
	{
		sCli = accept(sListen, (struct sockaddr *)&client,
								&iAddrSize);
		if(sCli == SOCKET_ERROR)
		{
			printf("accept Error[%d]\n",GetLastError());
			return -3;
		}

		printf("accept [%s:%d]\n", 
				inet_ntoa(client.sin_addr), 
				ntohs(client.sin_port));
		m_hWorkHand[workThreadCount++] = (void *)_beginthreadex(NULL, 0, workThread, (void* )sCli, 0, NULL);
	}

	return 0;
}