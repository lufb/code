#ifndef LINUXCODE
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <string.h>
#include <assert.h>
#include "argo_sock.h"
#include "argo_error.h"


/**
 *	InitNetWork -		初如化网络环境
 *	
 *	return
 *		==0			正常
 *		!=0			出错
 */
#ifndef LINUXCODE
int	MSocket::initNetWork()
{
	WSADATA						wsd;

	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		return BUILD_ERROR(_OSerrno(), 0);
	
	return 0;
}
#endif

void MSocket::destroy(void)
{	
#ifndef LINUXCODE
	::WSACleanup();
#endif
}

/**
 *	cleanup - 销毁网络环境
 *
 *	return
 *		无
 */
void MSocket::cleanup()
{
#ifndef LINUXCODE
	WSACleanup();
#endif
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
	if ( err == -1 )
	{
		return	BUILD_ERROR(_OSerrno(), E_ABORT1);
	}
	else if ( 0 == err )
		return BUILD_ERROR(0, E_RCVTIMEOUT);

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
			return BUILD_ERROR(_OSerrno(), E_SOCKCLOSE);
		else if ( errorcode == -1 )
		{
#ifndef		LINUXCODE
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
#else
			errorcode = _OSerrno();
			if(errorcode == EINTR)
				continue;
#endif
			return	BUILD_ERROR(errorcode, E_RCV);
		}

		rsize += errorcode;
	}

	return	0;
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
 *		==	0			成功(不需要通过select来探测)
 *		<	0			失败(remark)
 */
int MSocket::dnsToIP(char* strDns, size_t size, char *dst, size_t dstSize)
{
	assert(strDns != NULL && size > 0);
	
	struct hostent				*	lpstHost;
	sockaddr_in                     stAddr;
	//char						tmpBuffer[256] = {'\0'};
	
	if ( inet_addr(strDns) == INADDR_NONE )
	{
		if ( (lpstHost = gethostbyname(strDns)) == NULL )
		{
			//无效的域名
			dst[0] = '\0';
			return BUILD_ERROR(_OSerrno(), E_DNS);
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


void MSocket::close_sock(int s)
{
#ifdef WIN32
	closesocket(s);
#else
	close(s);
#endif
}	

int MSocket::block_connect(const char *ip, unsigned short port)
{
	int	s;
	struct sockaddr_in addr;
	
	if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	
	if(connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close_sock(s);
		return -1;
	}
	
	return s;
}

