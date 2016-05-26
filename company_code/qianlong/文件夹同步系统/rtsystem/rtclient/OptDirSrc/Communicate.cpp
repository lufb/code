/********************************************************************************************************
Copyright (C), 1988-2007, Qianlong Tech. Co., Ltd.

	FileName:		DirTran.cpp
	Author:			lufubo6
	Create:			2012.12.25
	
	Description:	RT通讯模块（仅文件夹传输才用了）
	History:		
*********************************************************************************************************/

#include "Communicate.h"

MCommunicateBaseIO::MCommunicateBaseIO():m_ClientSocket(INVALID_SOCKET), m_sCurrentServerPort(0),
												m_sCurrentServerPos(0),m_bConnectFlg(false)
{
	memset(m_strCurrentServerAdd, 0, sizeof(m_strCurrentServerAdd));
	Instance();
}

MCommunicateBaseIO::~MCommunicateBaseIO()
{

}

void MCommunicateBaseIO::Instance()
{
	ns_startup();
}

void MCommunicateBaseIO::ns_startup()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if(err != 0)
		return;	
	if(LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2 ) {
		WSACleanup( );
		return; 
	}
#endif
}

void MCommunicateBaseIO::ns_cleanup()
{
#ifdef WIN32
	WSACleanup();
#endif
}

//	>0	成功
int MCommunicateBaseIO::ns_connect2(unsigned int ip, unsigned short port)
{
	int	s;
	struct sockaddr_in addr;

	if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

	if(connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
#ifdef WIN32
		closesocket(s);
#else
		close(s);
#endif
		return -1;
	}

	return s;
}

int MCommunicateBaseIO::ns_connect(const char *ip, unsigned short port)
{
	int	s;
	struct sockaddr_in addr;

	if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	if(connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
#ifdef WIN32
		closesocket(s);
#else
		close(s);
#endif
		return -1;
	}

	return s;
}

int MCommunicateBaseIO::ns_recv(int s, void *buf, size_t len)
{
	int iRecvTotal = 0;     
	int iRecvOnce = 0;     
	while ( iRecvTotal < len )        
	{         
		iRecvOnce = ::recv( s, (char* )buf + iRecvTotal, len - iRecvTotal, 0 );         
		if ( iRecvOnce == SOCKET_ERROR || iRecvOnce == 0 )         
		{     
			return -1;         
		}         
		iRecvTotal += iRecvOnce;    
	} 

	return iRecvTotal;

}

int	MCommunicateBaseIO::ns_send(int s, void *buffer, size_t size)
{
	int rc;

	rc = send(s, (char *)buffer, size, 0);
	if(rc <= 0)	
		return -1;

	return rc;
}

void MCommunicateBaseIO::ns_close(int s)
{
	closesocket(s);
}


int MCommunicateBaseIO::ns_create()
{
	int s;

	s = socket(AF_INET, SOCK_STREAM, 0);

	return s;
}

int MCommunicateBaseIO::ns_bind(int s, unsigned short port)
{
	struct sockaddr_in	addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

#ifndef WIN32
	unsigned int	optval;
	struct	 linger	optval1;
	//  设置SO_REUSEADDR选项(服务器快速重起)
	optval   =   0x1;
	setsockopt(stSocket, SOL_SOCKET, SO_REUSEADDR, &optval, 4);
            
	//设置SO_LINGER选项(防范CLOSE_WAIT挂住所有套接字)
	optval1.l_onoff = 1;
	optval1.l_linger = 60;
	setsockopt(stSocket, SOL_SOCKET, SO_LINGER, &optval1, sizeof(struct linger));
#endif

	return bind(s, (struct sockaddr *)&addr, sizeof(addr));
}

int MCommunicateBaseIO::ns_set_recv_timeout(int s, unsigned int timeout)
{
#ifdef WIN32
	return setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(int));
#else
	struct timeval tv;

	tv.tv_sec  = timeout / 1000;
	tv.tv_usec = 0;
	return setsockopt(s, SOL_SOCKET,SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
#endif
}

void MCommunicateBaseIO::ns_set_socket(SOCKET s)
{
	m_ClientSocket = s;
}

const SOCKET MCommunicateBaseIO::ns_get_socket() const
{
	return m_ClientSocket;
}

int MCommunicateBaseIO::ns_set_currentserverpos(unsigned short port)
{
	if(port <=0 )
		return -1;

	m_sCurrentServerPos = port;
	return 0;
}
const unsigned short MCommunicateBaseIO::ns_get_currentserverpos() const
{
	return m_sCurrentServerPos;
}

int MCommunicateBaseIO::ns_set_currentserveradd(char *add, unsigned size)
{
	if(size > sizeof(m_strCurrentServerAdd))
		return -1;
	strncpy(m_strCurrentServerAdd, add, size);
	return 0;
}

const char* const MCommunicateBaseIO::ns_get_currentserveradd() const
{
	return m_strCurrentServerAdd;
}

void MCommunicateBaseIO::ns_set_currentserverport(unsigned short port)
{
	m_sCurrentServerPort = port;
}

const unsigned short MCommunicateBaseIO::ns_get_currentserverport() const
{
	return m_sCurrentServerPort;
}
void MCommunicateBaseIO::ns_set_connectflag(bool flag)
{
	m_bConnectFlg = flag;
}

const bool MCommunicateBaseIO::ns_get_connectflag() const
{
	return m_bConnectFlg;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

M4XCommunicateIO::M4XCommunicateIO():MCommunicateBaseIO()
{

}

M4XCommunicateIO::~M4XCommunicateIO()
{

}

int	M4XCommunicateIO::Create4XHead(tagComm_FrameHead* head, char *body, size_t bodySize, unsigned char market, 
 									unsigned char main, unsigned char child, short errorCode)
{
	MCRC16					mcrc16 ;

	assert(head != NULL);

	head->cMarket = market;
	head->sDataSize = bodySize;
	head->cMainType = main;
	head->cChildType = child;
	head->sErrorCode = errorCode;
	head->sRequestCode = 0;//
	head->sStatus = 0;//

	if(body == NULL || bodySize == 0)
		head->sCheckCode = 0;
	else
		head->sCheckCode = mcrc16.CheckCode(body, bodySize);

	return 0;
}


bool M4XCommunicateIO::RecvPacIsRight(char* buffer, size_t bufferSize, unsigned char main, unsigned char child)
{
	MCRC16					mcrc16 ;
	tagComm_FrameHead*		head = (tagComm_FrameHead* )buffer;

	if(bufferSize > COMM_MAX_FRAME_SIZE)
	{
		return false;
	}
		

	if(head->cMarket != '#')
	{	
		return false;
	}

	if(head->cMainType != main || head->cChildType != child)
	{
		return false;
	}

	if(head->sCheckCode != mcrc16.CheckCode(buffer+sizeof(tagComm_FrameHead), bufferSize-sizeof(tagComm_FrameHead)))
	{
		return false;
	}
	
	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////