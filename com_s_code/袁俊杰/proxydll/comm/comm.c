/**
 *	comm/comm.c
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了套接字连接过程
 *
 *	修改历史:
 *
 *	2013-01-07 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include "comm.h"

/**
 *	create_connect		- 创建连接
 *
 *	@addr			[in]服务器地址
 *	@port			[in]服务器端口
 *	@psockfd		[out]到服务器的套接字指针，输出参数
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int create_connect( IN char* addr, IN unsigned short port, OUT int* psockfd )
{
	int		res;

	*psockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( INVALID_SOCKET == *psockfd )
		return _MKERRNO(GetLastError(), 0);

	res = set_asynchronous_socket( *psockfd );
	if ( 0 != res )		return	res;

	res = _create_connect( *psockfd, addr, port );
	if ( 0 != res )		return	res;

	return 0;
}

/**
 *	send_data		- 发送数据
 *
 *	@sockfd			[in]套接字
 *	@buf			[in]发送数据缓存
 *	@size			[in]发送数据大小
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int send_data( int sockfd, char* buf, int size )
{
	int		rsize = 0;
	int		errorcode;
	while ( rsize != size )
	{
		errorcode = send( sockfd, buf + rsize, size - rsize, 0 );
		if ( errorcode == SOCKET_ERROR )
		{
			errorcode = _MKERRNO( GetLastError(), 0 );
			closesocket( sockfd );
			return	errorcode;
		}

		rsize += errorcode;
	}

	return	0;
}

/**
 *	recv_data		- 接收数据
 *
 *	@sockfd			[in]套接字
 *	@buf			[in]接收数据缓存
 *	@size			[in]接收数据大小
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 *
 *	remark:
 *			size参数指明了要收多少个字节，函数在收够这么多字节之前是不会返回的。
 *			内部的select函数有5秒的超时设置，如果超时就直接返回错误并关闭套接字。
 */
int recv_data( int sockfd, char* buf, int size )
{
	int				rsize = 0;
	int				flag, errorcode, res;

	while ( rsize != size )
	{
		errorcode = recv( sockfd, buf + rsize, size - rsize, 0 );
		if ( errorcode == 0 )
			return _MKERRNO( 0, ERR_CONNECT_CLOSEED);
		else if ( errorcode == SOCKET_ERROR )
		{
			errorcode = GetLastError();

			if ( WSAEWOULDBLOCK == errorcode )
			{
				flag	= SEL_R;
				res		= select_socket( &sockfd, &flag, 1, 5, 0 );
				if ( 0 != res )
				{
					closesocket( sockfd );
					return res;
				}
				else
					continue;
			}

			closesocket( sockfd );
			return	_MKERRNO( errorcode, 0 );
		}

		rsize += errorcode;
	}

	return	rsize;
}

/**
 *	select_socket		- 检查描述符状态
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
int select_socket( IN const int* pfd, IN int* pflag, IN int count, IN int sec, IN int usec )
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
			FD_SET( (const int)pfd[i], &fdr );
		if ( (pflag[i] & 0x2) )
			FD_SET( (const int)pfd[i], &fdw );
		if ( (pflag[i] & 0x4) )
			FD_SET( (const int)pfd[i], &fde );
	}

	tval.tv_sec		= sec;
	tval.tv_usec	= usec;

	err = select( maxfd1, &fdr, &fdw, &fde, &tval );
	if ( err == SOCKET_ERROR )
	{
		err = GetLastError();
		return	_MKERRNO( err, 0 );
	}
	else if ( 0 == err )
		return _MKERRNO( 0, ERR_SOCKET_TIMEOUT);

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
 *	single_select_and_send		- 单个套接字的写状态检测并发送
 *
 *	@sockfd			[in]套接字
 *	@buf			[in]待发送数据缓存
 *	@size			[in]发送缓存大小
 *	@sec			[in]超时时间(秒)
 *	@usec			[in]超时时间(微秒)
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int single_select_and_send( IN int sockfd, IN char* buf, IN int size, IN int sec, IN int usec )
{
	int		res, flag;
	
	flag	= SEL_W;
	res		= select_socket( &sockfd, &flag, 1, sec, usec );
	if ( 0 != res )	
	{
		closesocket( sockfd );
		return res;
	}

	if ( flag & 0x2 )
	{
		res = send_data( sockfd, buf, size );
		if ( 0 != res )		return res;
	}
	else
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_UNKNOW );
	}

	return 0;
}

/**
 *	single_select_and_send		- 单个套接字的写状态检测并发送
 *
 *	@sockfd			[in]套接字
 *	@buf			[in]待发接收缓存
 *	@size			[in]需要接收数据大小
 *	@sec			[in]超时时间(秒)
 *	@usec			[in]超时时间(微秒)
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int single_select_and_recv( IN int sockfd, IN char* buf, IN int size, IN int sec, IN int usec )
{
	int		res, flag;

	flag	= SEL_R;
	res		= select_socket( &sockfd, &flag, 1, sec, usec );
	if ( 0 != res )	
	{
		closesocket( sockfd );
		return res;
	}

	if ( flag & 0x1 )
	{
		res = recv_data( sockfd, buf, size );
		if ( 0 > res )		return res;
	}
	else
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_UNKNOW );
	}

	return 0;
}

/**
 *	set_asynchronous_socket		- 设置成异步套接字
 *
 *	@psockfd		[in]套接字
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int set_asynchronous_socket( IN int sockfd )
{
	unsigned long		errorcode;
	
	errorcode	= 1;
	if ( ioctlsocket(sockfd, FIONBIO, &errorcode) == SOCKET_ERROR )
	{
		errorcode = GetLastError();
		closesocket( sockfd );
		return _MKERRNO( errorcode, 0 );
	}

	return 0;
}

/**
 *	set_synchronous_socket		- 设置成同步套接字
 *
 *	@psockfd		[in]套接字
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int set_synchronous_socket( IN int sockfd )
{
	unsigned long		errorcode;
	
	errorcode = 0;
	if ( ioctlsocket(sockfd, FIONBIO, &errorcode) == SOCKET_ERROR )
	{
		errorcode = GetLastError();
		closesocket( sockfd );
		return _MKERRNO( errorcode, 0 );
	}

	return 0;
}

/**
 *	get_host_by_name		- 域名转换成ip
 *
 *	@addr			[in]域名地址
 *	@ip				[OUT]IP地址
 *
 *	remark:
 *			如果转换失败，ip的字符串为空
 */
void get_host_by_name( IN char* addr, OUT char* ip )
{
	struct hostent			*phost;
	struct in_addr			*haddr;

	// 如果传递的是域名，从DNS服务器获取IP
	phost = gethostbyname( addr );
	if ( phost && phost->h_addr_list[0] )
	{
		haddr = (struct in_addr *)phost->h_addr_list[0];
		strncpy( ip, inet_ntoa(*haddr), 16 );
	}
	else
		ip[0] = '\0';
}

/**
 *	_create_connect		- 创建连接（调用connect函数）
 *
 *	@sockfd			[in]到服务器的套接字
 *	@addr			[in]服务器地址
 *	@port			[in]服务器端口
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int _create_connect( IN int sockfd, IN char* addr, IN unsigned short port )
{
	char					IP[16] = {0};
	struct sockaddr_in		staddr;

	staddr.sin_family	= AF_INET;
	staddr.sin_port		= htons(port);

	// 如果传递的是域名，从DNS服务器获取IP
	get_host_by_name( addr, IP );
	if ( '\0' != IP[0] )
		staddr.sin_addr.s_addr	= inet_addr( IP );
	else
		staddr.sin_addr.s_addr	= inet_addr( addr );

	// 建立连接（异步套接字，不需要马上判断是否连接成功）
	connect( sockfd, (struct sockaddr*)&staddr, sizeof(staddr) );

	return 0;
}