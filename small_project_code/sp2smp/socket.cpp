#include "socket.h"
#include <winsock2.h>
#include "error_define.h"
#include "global.h"

int	
init_network()
{
	WSADATA						wsd;
	
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
		return BUILD_ERROR(_OSerrno(), 0);
	
	return 0;
}

void 
destroy_network(void)
{	
	::WSACleanup();
}

int
create_listen(unsigned short port)
{
	int							listen_sock;
	struct sockaddr_in			local;
	
	listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (listen_sock == SOCKET_ERROR)
		return BUILD_ERROR(_OSerrno(), 0);
	
	local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
	
	re_bind_sock(listen_sock);		/*设置重绑定*/

	if (bind(listen_sock, (struct sockaddr *)&local, 
		sizeof(local)) == SOCKET_ERROR)
		return BUILD_ERROR(_OSerrno(), 0);		/*注意：当端口被占用时，必须要返回这个错误码，用户才会重新设置监听端口*/
	
	return listen_sock;
}

int 
setnonblocking( int s)
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
 *	send_nonblock		- 非阻塞发送固定大小数据
 *
 *	@sockfd			[in]套接字
 *	@buf			[in]发送数据缓存
 *	@size			[in]发送数据大小
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，error.h
 */
int 
send_nonblock( int sockfd, char* buf, int size )
{
	int						rc;

	rc = send( sockfd, buf, size, 0 );
	if(rc != size)
		return BUILD_ERROR(_OSerrno(), 0);

	return	0;
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
int 
rcv_non_block( int sockfd, char* buf, int size, int timeout /*= 3*/)
{
	int				rsize = 0;
	int				flag, errorcode, res;

	while ( rsize != size )
	{
		errorcode = recv( sockfd, buf + rsize, size - rsize, 0 );
		if ( errorcode == 0 )
			return BUILD_ERROR(_OSerrno(), E_RCV);
		else if ( errorcode == SOCKET_ERROR )
		{
			errorcode = GetLastError();

			if ( WSAEWOULDBLOCK == errorcode )
			{
				flag	= 0x1;
				res		= select_socket( &sockfd, &flag, 1, timeout, 0 );
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
int select_socket(  const int* pfd,  int* pflag,  int count,  int sec, int usec )
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
		return	BUILD_ERROR(_OSerrno(), E_MYABORT);
	}
	else if ( 0 == err )
		return BUILD_ERROR(0, E_TIMEOUT);

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

int	
block_connect(char *ip, unsigned short port)
{
	int							err;
	struct	sockaddr_in			addrServer;

	if((err = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)
		return BUILD_ERROR(_OSerrno(), 0);

	addrServer.sin_family = AF_INET;
    addrServer.sin_addr.s_addr = inet_addr(ip);
    addrServer.sin_port = htons(port);
	
	if(connect(err,(const struct sockaddr *)&addrServer,sizeof(sockaddr)) != 0)
        return BUILD_ERROR(_OSerrno(), 0);

	return err;
}

void set_sock_buf(int sock, size_t sockBufSize)
{
	int			optVal = sockBufSize;
	int			optLen = sizeof(int);
	
	/** set recv buffer size*/
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, optLen);	
	/** set send buffer size*/
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&optVal, optLen);
}

void
re_bind_sock(int sock)
{
	unsigned int	optval;
	//  设置SO_REUSEADDR选项(服务器快速重起)
	optval   =   0x1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, 4);
}
	