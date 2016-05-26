/**
 *	comm/comm.c
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	�������׽������ӹ���
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#include "comm.h"

/**
 *	create_connect		- ��������
 *
 *	@addr			[in]��������ַ
 *	@port			[in]�������˿�
 *	@psockfd		[out]�����������׽���ָ�룬�������
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	send_data		- ��������
 *
 *	@sockfd			[in]�׽���
 *	@buf			[in]�������ݻ���
 *	@size			[in]�������ݴ�С
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	recv_data		- ��������
 *
 *	@sockfd			[in]�׽���
 *	@buf			[in]�������ݻ���
 *	@size			[in]�������ݴ�С
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 *
 *	remark:
 *			size����ָ����Ҫ�ն��ٸ��ֽڣ��������չ���ô���ֽ�֮ǰ�ǲ��᷵�صġ�
 *			�ڲ���select������5��ĳ�ʱ���ã������ʱ��ֱ�ӷ��ش��󲢹ر��׽��֡�
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
 *	select_socket		- ���������״̬
 *
 *	@pfd			[in]���д��������������ָ��
 *	@pflag			[in]������������ҪУ��ı�־λ���飨ÿ��Ԫ�ص�0λ���ɶ�У�飻1λ����дУ�飻2λ��errУ�飩
 *	@count			[in]�������Ÿ���
 *	@tval			[in]��ʱʱ�����ýṹ
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	single_select_and_send		- �����׽��ֵ�д״̬��Ⲣ����
 *
 *	@sockfd			[in]�׽���
 *	@buf			[in]���������ݻ���
 *	@size			[in]���ͻ����С
 *	@sec			[in]��ʱʱ��(��)
 *	@usec			[in]��ʱʱ��(΢��)
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	single_select_and_send		- �����׽��ֵ�д״̬��Ⲣ����
 *
 *	@sockfd			[in]�׽���
 *	@buf			[in]�������ջ���
 *	@size			[in]��Ҫ�������ݴ�С
 *	@sec			[in]��ʱʱ��(��)
 *	@usec			[in]��ʱʱ��(΢��)
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	set_asynchronous_socket		- ���ó��첽�׽���
 *
 *	@psockfd		[in]�׽���
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	set_synchronous_socket		- ���ó�ͬ���׽���
 *
 *	@psockfd		[in]�׽���
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
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
 *	get_host_by_name		- ����ת����ip
 *
 *	@addr			[in]������ַ
 *	@ip				[OUT]IP��ַ
 *
 *	remark:
 *			���ת��ʧ�ܣ�ip���ַ���Ϊ��
 */
void get_host_by_name( IN char* addr, OUT char* ip )
{
	struct hostent			*phost;
	struct in_addr			*haddr;

	// ������ݵ�����������DNS��������ȡIP
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
 *	_create_connect		- �������ӣ�����connect������
 *
 *	@sockfd			[in]�����������׽���
 *	@addr			[in]��������ַ
 *	@port			[in]�������˿�
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int _create_connect( IN int sockfd, IN char* addr, IN unsigned short port )
{
	char					IP[16] = {0};
	struct sockaddr_in		staddr;

	staddr.sin_family	= AF_INET;
	staddr.sin_port		= htons(port);

	// ������ݵ�����������DNS��������ȡIP
	get_host_by_name( addr, IP );
	if ( '\0' != IP[0] )
		staddr.sin_addr.s_addr	= inet_addr( IP );
	else
		staddr.sin_addr.s_addr	= inet_addr( addr );

	// �������ӣ��첽�׽��֣�����Ҫ�����ж��Ƿ����ӳɹ���
	connect( sockfd, (struct sockaddr*)&staddr, sizeof(staddr) );

	return 0;
}