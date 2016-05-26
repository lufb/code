#include <winsock2.h>
#include <assert.h>
#include "MSocket.h"
#include "error.h"


/**
 *	InitNetWork -		初如化网络环境
 *	
 *	return
 *		==0			正常
 *		!=0			出错
 */
int	MSocket::initNetWork()
{
	WSADATA						wsd;

	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		return BUILD_ERROR(_OSerrno(), 0);
	
	return 0;
}

void MSocket::destroy(void)
{	
	::WSACleanup();
}

/**
 *	cleanup - 销毁网络环境
 *
 *	return
 *		无
 */
void MSocket::cleanup()
{
	WSACleanup();
}


/**
 *	fastClose	-		快速关闭SOCKET RST包
 *	
 *	@s			[in]	需要关闭的套接字
 *
 *	return
 *		0		成功
 *		!0		失败
 *
 */
int MSocket::fastClose(SOCKET s)
{
	struct linger opt;

	if(s == INVALID_SOCKET)
		return 0;

	opt.l_onoff	= 1;
	opt.l_linger= 0;

	setsockopt(s, SOL_SOCKET, SO_LINGER,
			(const char*)&opt, sizeof(opt));

	return closesocket(s);
}

/**
 *	close		 -		关闭socket
 *	
 *	@s			[in]	需要关闭的套接字
 *
 *	return
 *		0		成功
 *		!0		失败
 *
 */
int MSocket::close(SOCKET s)
{
	if(s == INVALID_SOCKET)
		return 0;
	shutdown(s, SD_SEND);
	return closesocket(s);
}

/**
 *	setSockBuf	-	设置套接字的发送缓冲区与接收缓冲区
 *
 *	@sock:		[in]	需要设置的套接字
 *	@sockBufSize:[in]	缓冲区大小
 *	return
 *				==0		成功
 *				< 0	失败
 */
int MSocket::setSockBuf(SOCKET sock, size_t sockBufSize)
{
	int			optVal = sockBufSize;
	int			optLen = sizeof(int);

	/** set recv buffer size*/
	if(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, optLen) == SOCKET_ERROR)	
		return BUILD_ERROR(_OSerrno(), E_OS);
	/** set send buffer size*/
	if(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&optVal, optLen) == SOCKET_ERROR) 
		return BUILD_ERROR(_OSerrno(), E_OS);

	return 0;
}

/**
 *	ne_getsock_errno - 根据SOCKET描述符,得到指定SOCKET的最后一次错误码
 *
 *	@sock:		[in]	套接字
 *
 *	return
 *		>	0	错误代码
 *		<	0	remark
 *
 */
int MSocket::getsockError( SOCKET s)
{
	int errorcode = -2;
	int len = sizeof(int);

	if(!getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&errorcode, (int *)&len))
		return errorcode;
	else
		return -1;
}

/**
 *	setnonblocking - 设置SOCKET非阻塞
 *
 *	@s:		[in]	SOCKET描述符
 *
 *	return
 *		0		成功
 *		!0		失败
 *
 */
int MSocket::setnonblocking( SOCKET s)
{
	int				rc;
	unsigned long	arg = 1;

	/**
	 *	设置非阻塞方式连接
	 */
	rc = ioctlsocket(s, FIONBIO, (unsigned long*)&arg);
	if(rc == SOCKET_ERROR)
		return -1;

	return 0;
}

/**
 *	setBlocking - 设置SOCKET阻塞
 *
 *	@s:		[in]	SOCKET描述符
 *
 *	return
 *		0		成功
 *		!0		失败
 *
 */
int MSocket::setBlocking(SOCKET s)
{
	int				rc;
	unsigned long	arg = 0;

	/**
	 *	设置非阻塞方式连接
	 */
	rc = ioctlsocket(s, FIONBIO, (unsigned long*)&arg);
	if(rc == SOCKET_ERROR)
		return -1;

	return 0;
}

/**
 *	sendData		- 非阻塞发送固定大小数据
 *
 *	@sockfd			[in]套接字
 *	@buf			[in]发送数据缓存
 *	@size			[in]发送数据大小
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，error.h
 */
int MSocket::sendDataNonBlock( int sockfd, char* buf, int size )
{
	int						rc;

	rc = send( sockfd, buf, size, 0 );
	if(rc != size)
		return BUILD_ERROR(_OSerrno(), 0);

	return	0;
}

/**
 *	selectSocket		- 检查描述符状态
 *
 *	@pfd			[in]所有待检查描述符数组指针
 *	@pflag			[in]所有描述符需要校验的标志位数组（每个元素的0位：可读校验；1位：可写校验；2位：err校验）
 *	@count			[in]描述符号个数
 *	@tval			[in]超时时间设置结构
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int MSocket::selectSocket( IN const int* pfd, IN int* pflag, IN int count, IN int sec, IN int usec )
{
	int					i, err, maxfd1 = 0;
	fd_set				fdr, fdw, fde;
	struct timeval		tval;

	FD_ZERO( &fdr );
	FD_ZERO( &fdw );
	FD_ZERO( &fde );

	for ( i = 0; i < count; i++ )
	{
		if ( pfd[i] > maxfd1 )
			maxfd1 = pfd[i];

		if ( (pflag[i] & 0x1) )
			FD_SET( (const unsigned int)pfd[i], &fdr );
		if ( (pflag[i] & 0x2) )
			FD_SET( (const unsigned int)pfd[i], &fdw );
		if ( (pflag[i] & 0x4) )
			FD_SET( (const unsigned int)pfd[i], &fde );
	}

	tval.tv_sec		= sec;
	tval.tv_usec	= usec;

	err = select( maxfd1 + 1, &fdr, &fdw, &fde, &tval );
	if ( err == SOCKET_ERROR )
	{
		return	BUILD_ERROR(_OSerrno(), EABORT);
	}
	else if ( 0 == err )
		return BUILD_ERROR(0, ERCVTIMEOUT);

	for ( i = 0; i < count; i++ )
	{
		if ( pflag[i] & 0x1 )
			if ( !FD_ISSET( (const int)pfd[i], &fdr ) )
				pflag[i] &= (~0x1);
		if ( pflag[i] & 0x2 )
			if ( !FD_ISSET( (const int)pfd[i], &fdw ) )
				pflag[i] &= (~0x2);
		if ( pflag[i] & 0x4 )
			if ( !FD_ISSET( (const int)pfd[i], &fde ) )
				pflag[i] &= (~0x4);
	}

	return 0;
}

/**
 *	recv_data		- 非阻塞接收固定大小数据，在超时时间(timeout)内不接收完不返回
 *
 *	@sockfd			[in]	套接字
 *	@buf			[in]	接收数据缓存
 *	@size			[in]	接收数据大小
 *	@timeout		[in]	接收的超时时间(秒)，默认为3
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，error.h的错误码
 *
 */
int MSocket::rcvDataNonBlock( int sockfd, char* buf, int size, int timeout /*= 3*/)
{
	int				rsize = 0;
	int				flag, errorcode, res;

	while ( rsize != size )
	{
		errorcode = recv( sockfd, buf + rsize, size - rsize, 0 );
		if ( errorcode == 0 )
			return BUILD_ERROR(_OSerrno(), ESOCKCLOSE);
		else if ( errorcode == SOCKET_ERROR )
		{
			errorcode = GetLastError();

			if ( WSAEWOULDBLOCK == errorcode )
			{
				flag	= 0x1;
				res		= selectSocket( &sockfd, &flag, 1, timeout, 0 );
				if ( 0 != res )
				{
					return res;
				}
				else
					continue;
			}

			return	BUILD_ERROR(errorcode, 0);
		}

		rsize += errorcode;
	}

	return	0;
}

/**
 *	asynConnect - 异步连接
 *
 *	@ip:				IP地址(big endian)
 *	@port:				PORT端口(big endian)
 *	@s:					创建好的SOCKET描述符
 *
 *	return
 *		>	0			成功(需要继续通过select来探测SOCKET是否已经可写)
 *		==	0			成功(不需要通过select来探测)
 *		<	0			失败(remark)
 *
 *	remark:
 *		a specific error code can be retrieved
 *	by calling GetLastError
 */
int MSocket::asynConnect( unsigned int ip,
	 unsigned short port,  unsigned int *s)
{
	int					rc, _s;
	struct sockaddr_in	addr;

	*s = INVALID_SOCKET;

	/**
	 *	创建一个SOCKET描述符
	 */
	_s = socket(AF_INET, SOCK_STREAM, 0);
	if(_s == INVALID_SOCKET)
		return -1;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = port;

	/**
	 *	设置非阻塞方式连接
	 */
	rc = setnonblocking(_s);
	if(rc) {
		close(_s);
		return -2;
	}

	/**
	 *	连接服务器
	 */
	if(connect(_s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		if(WSAEWOULDBLOCK == GetLastError()) {
			*s = _s;
			return 1;
		} else {
			close(_s);
			return -3;
		}
	} else {
		*s = _s;
		return 0;
	}
}


/**
 *	dnsToIP -			将域名转化为IP地址
 *
 *	@strDns:			传入的待转换的dns域名(或者是IP地址)
 *	@size:				传入的字符串长度
 *	@dst:				存放结果的地址
 *	@dstSize:			存放结果的空间长度
 *
 *	return
 *		==	0			成功
 *		<	0			失败(remark)
 */
int MSocket::dnsToIP(const char* strDns, size_t size, char *dst, size_t dstSize)
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
			return BUILD_ERROR(_OSerrno(), EDNS);
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

/**
 *	setSock		-			设置套接字的属性(非阻塞，发送与接收缓冲区大小)
 *
 *	@sock			[in]		需要设置的套接字
 *	@bufSize		[in]		缓冲区大小
 *
 *	return
 *								无
 */
void MSocket::setSock(SOCKET sock, size_t bufSize)
{
	assert(sock != INVALID_SOCKET);

	setnonblocking(sock);
	setSockBuf(sock, bufSize);
}

