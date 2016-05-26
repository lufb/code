/*
		文件：Proxy.cpp
		功能：代理客户端实现
		时间：2007.5.13
		作者：luozn
		版权: 上海乾隆高科技版权所有

		修改纪录
		修改日期		修改人员	修改内容和原因
*/

#include "Global.h"
#include "./Proxy.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxy::MProxy() : m_socket(INVALID_SOCKET)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxy::~MProxy()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MProxy::Release()
{
	if( m_socket != INVALID_SOCKET )
	{
		closesocket( m_socket );
		m_socket = INVALID_SOCKET;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxy::ConnectToProxy( const char * address, unsigned short port )
{
	int nErrorCode = 0;

	nErrorCode = MSocket::CreateSocket( &m_socket, SOCK_STREAM );
	if( nErrorCode <= 0 )
	{
		Release();
		return nErrorCode;
	}

	nErrorCode = MSocket::SyncConnect( m_socket, address, port );
	if( nErrorCode < 0 )
	{
		Release();
		return nErrorCode;
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxy::SendToProxy( const char * Inbuf, int nSize )
{
	int				nErrorCode = -1;
	if( m_socket == INVALID_SOCKET )
	{
		return -1;
	}
	
	nErrorCode = MSocket::Send( m_socket, Inbuf, nSize );

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SOCKET MProxy::GetSocket() const
{
	return m_socket;
}

int	MProxy::LoadCfg(int	SvrSerial)
{
	memset(&m_Proxy, 0, sizeof(tagGetProxyRes));
	if(Global_Option.GetProxy(&m_Proxy) < 0)
		return -1;

	m_SrvIP = Global_Option.GetSrvIP(SvrSerial);
	m_SrvPort = Global_Option.GetPort(SvrSerial);

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxySock5::MProxySock5()
{
	m_socket = INVALID_SOCKET;
	m_Socket2Type = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxySock5::~MProxySock5()
{
	Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxy::ProxyType MProxySock5::GetProxyType() const
{
	return SOCKS5PROXY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock5::SendSock5Request0()
{
	char		tempbuf[256] = {0};

	tempbuf[0] = 0x05;			//sock5
	tempbuf[1] = 0x02;			////两种方式无需验证	
	tempbuf[2] = 0x00;			//无需验证	
	tempbuf[3] = 0x02;			//用户+密码

	return SendToProxy( tempbuf, 4 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock5::RecvSock5Answer0()
{	
	int				nErrorCode = -1;
	char			tempbuf[256] = {0};
	unsigned long	ulTimeOut = 5000;

	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	nErrorCode = MSocket::Recv( m_socket, tempbuf, 256 );
	if( nErrorCode > 0 )
	{
		if( tempbuf[0] != 0x5 )
		{
			Release();
		}
		if( tempbuf[1] != 0 && tempbuf[1] != 2 )
		{
			Release();	
		}
		m_Socket2Type = tempbuf[1];
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	MProxySock5::SendSock5Request1()
{
	int				nErrorCode = -1;
	char			tempbuf[256] = {0};

	if( m_Socket2Type <= 0 )
	{
		return m_Socket2Type;
	}
	
	if( m_Socket2Type != 0x2 )
	{
		return -1;
	}

	nErrorCode = 0;
	tempbuf[0] = 0x01;
	++nErrorCode;
	tempbuf[1] = strlen(m_Proxy.user );
	++nErrorCode;
	memcpy( &tempbuf[2], m_Proxy.user, tempbuf[1] );
	nErrorCode += tempbuf[1];
	tempbuf[nErrorCode] = strlen( m_Proxy.pwd);
	++nErrorCode;
	memcpy( &tempbuf[nErrorCode],  m_Proxy.pwd, tempbuf[nErrorCode-1] );
	nErrorCode += tempbuf[nErrorCode-1];

	nErrorCode = SendToProxy( tempbuf, nErrorCode );
	if( nErrorCode <  0 )
	{
		Release();
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock5::RecvSock5Answer1()
{
	int				nErrorCode = -1;
	char			tempbuf[256] = {0};
	unsigned long	ulTimeOut = 5000;

	if( m_Socket2Type != 0x2 )
	{
		return 0;
	}

	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	nErrorCode = MSocket::Recv( m_socket, tempbuf, 256 );
	if( nErrorCode > 0 )
	{
		if( tempbuf[0] != 0x1 )
		{
			Release();
		}
		if( tempbuf[1] != 0 )
		{
			Release();	
		}
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock5::SendCmd()
{
	int				nErrorCode = -1;
	char			tempbuf[256] = {0};
	unsigned short  port = 0;
	unsigned long	addr = 0;
	MString			ipaddr;
	
	tempbuf[0] = 0x5;
	tempbuf[1] = 0x1;
	tempbuf[2] = 0x0;
	tempbuf[3] = 0x1;

	ipaddr = MSocket::DNSToIP( m_SrvIP.c_str() );
	port = htons( m_SrvPort );

	if( ipaddr != "" )
	{
		addr = inet_addr( ipaddr.c_str() );
		memcpy( tempbuf+4, &addr, 4 );
		memcpy( tempbuf+8, &port, 2 );
		nErrorCode = SendToProxy( tempbuf, 10 );
		if( nErrorCode <  0 )
		{
			Release();
		}
	}
	else
	{	//本地无法解析域名
		tempbuf[3] = 0x3;
		tempbuf[4] = m_SrvIP.GetLength();
		memcpy( tempbuf+5, m_SrvIP.c_str(), tempbuf[4] );
		memcpy( tempbuf+5+tempbuf[4], &port, 2 );

		nErrorCode = SendToProxy( tempbuf, 5+tempbuf[4]+2 );
		if( nErrorCode <  0 )
		{
			Release();
		}
	}
	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock5::RecvCmd()
{
	int				nErrorCode = -1;
	char			tempbuf[256] = {0};
	unsigned long	ulTimeOut = 5000;

	if( m_Socket2Type != 0x2 && m_Socket2Type != 0 )
	{
		return -1;
	}

	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	nErrorCode = MSocket::Recv( m_socket, tempbuf, 256 );
	if( nErrorCode > 0 )
	{
		if( tempbuf[0] != 0x5 )
		{
			Release();
			return -1;
		}
		if( tempbuf[1] != 0 )
		{
			Release();
			return -1;
		}
	}
	else
	{
		Release();
	}	

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock5::Instance(int	SvrSerial)
{
	int			nErrorCode = -1;

	Release();

	if(LoadCfg(SvrSerial) < 0)
		return -1;

	nErrorCode = ConnectToProxy( m_Proxy.ip, m_Proxy.port);
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	
	nErrorCode = SendSock5Request0();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	nErrorCode = RecvSock5Answer0();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}

	nErrorCode = SendSock5Request1();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	nErrorCode = RecvSock5Answer1();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	
	nErrorCode = SendCmd();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	nErrorCode = RecvCmd();
	if( nErrorCode <= 0 )
	{
		return nErrorCode;
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxySock4::MProxySock4()
{
	m_socket = INVALID_SOCKET;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxySock4::~MProxySock4()
{
	Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock4::Instance(int	SvrSerial)
{
	int			nErrorCode = -1;
	
	Release();
	if(LoadCfg(SvrSerial) < 0)
		return -1;

	nErrorCode = ConnectToProxy( m_Proxy.ip, m_Proxy.port);
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	
	nErrorCode = SendCmd();
	if( nErrorCode < 0 )
	{
		Release();
		return nErrorCode;
	}
	nErrorCode = RecvCmd();
	if( nErrorCode <= 0 )
	{
		return nErrorCode;
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxy::ProxyType MProxySock4::GetProxyType() const
{
	return SOCKS4PROXY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock4::SendCmd()
{
	int				nErrorCode = -1;
	char			tempbuf[256] = {0};
	unsigned short  port = 0;
	unsigned long	addr = 0;
	MString			ipaddr;
	
	tempbuf[0] = 0x4;
	tempbuf[1] = 0x1;

	ipaddr = MSocket::DNSToIP( m_SrvIP.c_str() );
	port = htons( m_SrvPort);
	memcpy( tempbuf+2, &port, 2 );

	if( ipaddr != "" )
	{
		addr = inet_addr( ipaddr.c_str() );
		memcpy( tempbuf+4, &addr, 4 );

		tempbuf[8] = 0;

		nErrorCode = SendToProxy( tempbuf, 9 );
		if( nErrorCode <  0 )
		{
			Release();
		}
	}
	else
	{	//本地无法解析域名
		tempbuf[4] = 0;
		tempbuf[5] = 0;
		tempbuf[6] = 0;
		tempbuf[7] = 0xF;
		tempbuf[8] = 0;
		my_strncpy( tempbuf + 9, m_SrvIP.c_str(), 245 );

		nErrorCode = SendToProxy( tempbuf, 256 );
		if( nErrorCode <  0 )
		{
			Release();
		}
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxySock4::RecvCmd()
{
	int				nErrorCode = -1;
	char			tempbuf[256] = {0};
	unsigned long	ulTimeOut = 5000;


	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	nErrorCode = MSocket::Recv( m_socket, tempbuf, 256 );
	if( nErrorCode > 0 )
	{
		if( tempbuf[0] != 0x0 )
		{
			Release();
			return -1;
		}
		if( tempbuf[1] != 90 )
		{
			Release();
			return -1;
		}
	}
	else
	{
		Release();
	}	

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxyHttp::MProxyHttp()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxyHttp::~MProxyHttp()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxy::ProxyType MProxyHttp::GetProxyType() const
{
	return HTTP1_1PROXY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyHttp::Instance(int	SvrSerial)
{
	int			nErrorCode = -1;
	
	Release();
	if(LoadCfg(SvrSerial) < 0)
		return -1;

	nErrorCode = ConnectToProxy( m_Proxy.ip, m_Proxy.port);
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}

	nErrorCode = SendRequest();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	nErrorCode = RecvRequest();
	if( nErrorCode <= 0 )
	{
		return nErrorCode;
	}


	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyHttp::Base64Encode( const char * InBuf, char * Outbuf, int size )
{
	char	base64_encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int		buflen = 0; 


	while( size>0 )
	{
		*Outbuf++ = base64_encoding[ (InBuf[0] >> 2 ) & 0x3f];
		if( size>2 )
		{
			*Outbuf++ = base64_encoding[((InBuf[0] & 3) << 4) | (InBuf[1] >> 4)];
			*Outbuf++ = base64_encoding[((InBuf[1] & 0xF) << 2) | (InBuf[2] >> 6)];
			*Outbuf++ = base64_encoding[InBuf[2] & 0x3F];
		}
		else
		{
			switch( size )
			{
			case 1:
				*Outbuf++ = base64_encoding[(InBuf[0] & 3) << 4 ];
				*Outbuf++ = '=';
				*Outbuf++ = '=';
				break;
			case 2: 
				*Outbuf++ = base64_encoding[((InBuf[0] & 3) << 4) | (InBuf[1] >> 4)]; 
				*Outbuf++ = base64_encoding[((InBuf[1] & 0x0F) << 2) | (InBuf[2] >> 6)]; 
				*Outbuf++ = '='; 
				break; 
			} 
		} 
		InBuf +=3; 
		size -=3; 
		buflen +=4; 
	} 
	*Outbuf = 0; 
	return buflen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyHttp::SendRequest()
{
	int			nErrorCode = -1;
	char        tempbuf[1024] = {0};
	char		base64buf[1024] = {0};

	sprintf( tempbuf, "CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\n\r\n", m_SrvIP.c_str(), m_SrvPort,
		m_SrvIP.c_str(), m_SrvPort);

	if( m_Proxy.user[0] != 0)
	{	//需要用户名和密码验证 
		MString user = m_Proxy.user;
		MString passport = user + ":" + m_Proxy.pwd;
		//base64 trans
		nErrorCode = Base64Encode( passport.c_str(), base64buf, passport.GetLength() );
//		assert( nErrorCode == ( passport.GetLength()<<2 )/3 );

		strcat( tempbuf, "Authorization: Basic " );
		strcat( tempbuf, base64buf );
		strcat( tempbuf, "\r\n" );	
		strcat( tempbuf, "Proxy-Authorization: Basic " );
		strcat( tempbuf, base64buf );
		strcat( tempbuf, "\r\n\r\n" );
	}

	nErrorCode = SendToProxy( tempbuf, strlen( tempbuf ) );
	if( nErrorCode <  0 )
	{
		Release();
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyHttp::RecvRequest()
{
	int				nErrorCode = -1;
	MString			recvstring;
	char			tempbuf[8192] = {0};
	unsigned long	ulTimeOut = 5000;
	MCounter		counter;


	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	counter.SetCurTickCount();
	while( 1 )
	{
		if( counter.GetDuration() > 10000 )
		{
			break;
		}
		nErrorCode = MSocket::Recv( m_socket, tempbuf, 8190 );
		if( nErrorCode > 0 )
		{
			tempbuf[nErrorCode] = 0;
			recvstring += tempbuf;
			recvstring.Lower();
			if( memcmp( recvstring.c_str(), "http/", 5 ) )
			{
				Release();
				return -1;
			}

			if( recvstring.StringPosition( "\r\n" ) < 0 )
			{
				nErrorCode = recvstring.StringPosition( " " );
				if( nErrorCode < 0 )
				{
					Release();
					return -1;
				}
				else if( recvstring[nErrorCode=1] != 2 )
				{
					return -2;
				}
			}
			if( recvstring.GetLength() > 4 && (recvstring.StringPosition("200") >=0))
			{
				return 1;
			}

		}
		else
		{
			Release();
			break;
		}	

	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxyUsb::MProxyUsb()
{}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxyUsb::~MProxyUsb()
{}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxy::ProxyType MProxyUsb::GetProxyType() const
{
	return QL_USBPROXY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyUsb::Instance(int	SvrSerial)
{
	int			nErrorCode = -1;
	
	Release();
	if(LoadCfg(SvrSerial) < 0)
		return -1;

	nErrorCode = ConnectToProxy( m_Proxy.ip, m_Proxy.port);
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}

	nErrorCode = SendRequest0();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	nErrorCode = RecvRequest0();
	if( nErrorCode <= 0 )
	{
		return nErrorCode;
	}

	nErrorCode = SendRequest1();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	nErrorCode = RecvRequest1();
	if( nErrorCode <= 0 )
	{
		return nErrorCode;
	}


	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyUsb::SendRequest0()
{
	int			nErrorCode = -1;
	char        tempbuf[1024] = {0};
	VerHead *	pHead = NULL;
	CerHead *	pCerHead = NULL;

	pHead = reinterpret_cast<VerHead *>(tempbuf);
	pCerHead = reinterpret_cast<CerHead *>(tempbuf+sizeof(VerHead));
	pHead->Flag = '#';
	pHead->Ver = 0x02;
	pHead->Type = 0;
	pHead->DLength = sizeof(CerHead);

	nErrorCode = sizeof(VerHead);
	//目前只支持无认证方式
	pCerHead->Method = 0;
	pCerHead->PasswdLen = pCerHead->UsernameLen = 0;
	pCerHead->CheckCode = 0;
	nErrorCode += sizeof(CerHead);

	nErrorCode = SendToProxy( tempbuf, nErrorCode );
	if( nErrorCode < 0 )
	{
		Release();
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyUsb::RecvRequest0()
{
	int				nErrorCode = -1;
	char			tempbuf[1024] = {0};
	unsigned long	ulTimeOut = 5000;
	int				nRevlen = 0;
	VerHead *		pHead = NULL;
	CerReply *		pCerHead = NULL;


	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	nRevlen = 0;
	while( nRevlen < sizeof(VerHead)+sizeof(CerReply) )
	{
		nErrorCode = MSocket::Recv( m_socket, tempbuf+nRevlen, sizeof(VerHead)+sizeof(CerReply)-nRevlen );
		if( nErrorCode > 0 )
		{
			nRevlen += nErrorCode;
		}
		else
		{
			Release();
			return nErrorCode;
		}	
	}

	pHead = reinterpret_cast<VerHead *>(tempbuf);
	pCerHead = reinterpret_cast<CerReply *>(tempbuf+sizeof(VerHead));
	if( pCerHead->Reply != 0 )
	{	//认证失败
		Release();
		return -1;
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyUsb::SendRequest1()
{
	int			nErrorCode = -1;
	char        tempbuf[1024] = {0};
	VerHead *	pHead = NULL;
	ConHead *	pConnHead = NULL;
	MString		ipaddr;

	pHead = reinterpret_cast<VerHead *>(tempbuf);
	pConnHead = reinterpret_cast<ConHead *>(tempbuf+sizeof(VerHead));
	pHead->Flag = '#';
	pHead->Ver = 0x02;
	pHead->Type = 0x2;
	pHead->DLength = sizeof(ConHead);

	nErrorCode = sizeof(VerHead);


	ipaddr = MSocket::DNSToIP( m_SrvIP.c_str() );
	if( ipaddr == "" )
	{
		Release();
		return -1;
	}
	pConnHead->DesPort = htons( m_SrvPort);
	pConnHead->DesIp.s_addr = inet_addr( ipaddr.c_str() );
	pConnHead->CheckCode = 0;

	nErrorCode += sizeof(ConHead);
	nErrorCode = SendToProxy( tempbuf, nErrorCode );
	if( nErrorCode < 0 )
	{
		Release();
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyUsb::RecvRequest1()
{
	int				nErrorCode = -1;
	char			tempbuf[1024] = {0};
	unsigned long	ulTimeOut = 5000;
	int				nRevlen = 0;
	VerHead *		pHead = NULL;
	CerReply *		pCerHead = NULL;


	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	nRevlen = 0;
	while( nRevlen < sizeof(VerHead)+sizeof(CerReply) )
	{
		nErrorCode = MSocket::Recv( m_socket, tempbuf+nRevlen, sizeof(VerHead)+sizeof(CerReply)-nRevlen );
		if( nErrorCode > 0 )
		{
			nRevlen += nErrorCode;
		}
		else
		{
			Release();
			return nErrorCode;
		}	
	}

	pHead = reinterpret_cast<VerHead *>(tempbuf);
	pCerHead = reinterpret_cast<CerReply *>(tempbuf+sizeof(VerHead));
	if( pCerHead->Reply != 0 )
	{	//代理失败
		Release();
		return -1;
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxyGC::MProxyGC()
{}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxyGC::~MProxyGC()
{}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MProxy::ProxyType MProxyGC::GetProxyType() const
{
	return QL_USBPROXY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyGC::Instance(int	SvrSerial)
{
	int			nErrorCode = -1;
	
	Release();
	if(LoadCfg(SvrSerial) < 0)
		return -1;

	nErrorCode = ConnectToProxy( m_Proxy.ip, m_Proxy.port);
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
/*
	nErrorCode = SendRequest0();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	nErrorCode = RecvRequest0();
	if( nErrorCode <= 0 )
	{
		return nErrorCode;
	}
*/
	nErrorCode = SendRequest1();
	if( nErrorCode < 0 )
	{
		return nErrorCode;
	}
	nErrorCode = RecvRequest1();
	if( nErrorCode <= 0 )
	{
		return nErrorCode;
	}


	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyGC::SendRequest0()
{
	int			nErrorCode = -1;
	char        tempbuf[1024] = {0};
	tagQLGCProxy_Apply *	pHead = NULL;

	pHead = reinterpret_cast<tagQLGCProxy_Apply *>(tempbuf);
	memcpy(pHead->szFlag, "QLGC", 4);
	pHead->ucApplyType = 0;
	pHead->usSize = 0;

	nErrorCode = sizeof(tagQLGCProxy_Apply);

	nErrorCode = SendToProxy( tempbuf, nErrorCode );
	if( nErrorCode < 0 )
	{
		Release();
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyGC::RecvRequest0()
{
	int				nErrorCode = -1;
	char			tempbuf[1024] = {0};
	unsigned long	ulTimeOut = 5000;
	int				nRevlen = 0;
	tagQLGCProxy_Info *		pHead = NULL;

	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	nRevlen = 0;
	while( nRevlen < sizeof(tagQLGCProxy_Info)+sizeof(tagQLGCProxy_Apply) )
	{
		nErrorCode = MSocket::Recv( m_socket, tempbuf+nRevlen, sizeof(tagQLGCProxy_Info)+sizeof(tagQLGCProxy_Apply)-nRevlen );
		if( nErrorCode > 0 )
		{
			nRevlen += nErrorCode;
		}
		else
		{
			Release();
			return nErrorCode;
		}	
	}

	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyGC::SendRequest1()
{
	int			nErrorCode = -1;
	char        tempbuf[1024] = {0};
	int			sendsize;
	tagQLGCProxy_Apply * pApply = (tagQLGCProxy_Apply *)tempbuf;
	tagQLGCProxy_Certify * pcer = (tagQLGCProxy_Certify *)(tempbuf + sizeof(tagQLGCProxy_Apply));
	
	sendsize = sizeof(tagQLGCProxy_Certify) + sizeof(tagQLGCProxy_Apply);

	memset(tempbuf, 0, sendsize);
	memcpy(pApply->szFlag, "QLGC", 4);
	pApply->ucApplyType = 1;
	pApply->usSize = sizeof(tagQLGCProxy_Certify);
	memcpy(pcer->strIPAddress, m_SrvIP.c_str(), m_SrvIP.GetLength());
	pcer->usPort = m_SrvPort;

	nErrorCode = sendsize;
	nErrorCode = SendToProxy( tempbuf, nErrorCode );
	if( nErrorCode < 0 )
	{
		Release();
	}

	return nErrorCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MProxyGC::RecvRequest1()
{
	int				nErrorCode = -1;
	char			tempbuf[1024] = {0};
	unsigned long	ulTimeOut = 5000;
	int				nRevlen = 0;
	tagQLGCProxy_Apply		* phead;
	tagQLGCProxy_CertifyRet * pret;

	setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ulTimeOut, sizeof( ulTimeOut ) );

	nRevlen = 0;
	while( nRevlen < sizeof(tagQLGCProxy_CertifyRet)+sizeof(tagQLGCProxy_Apply) )
	{
		nErrorCode = MSocket::Recv( m_socket, tempbuf+nRevlen, sizeof(tagQLGCProxy_CertifyRet)+sizeof(tagQLGCProxy_Apply)-nRevlen );
		if( nErrorCode > 0 )
		{
			nRevlen += nErrorCode;
		}
		else
		{
			Release();
			return nErrorCode;
		}	
	}

	phead = (tagQLGCProxy_Apply * )tempbuf;
	pret = (tagQLGCProxy_CertifyRet *)(tempbuf + sizeof(tagQLGCProxy_Apply));

	if(phead->ucApplyType != 1)
		return -1;

	if(pret->ucReply < 0)
		return -1;
	
	return 1;
}