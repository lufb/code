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
	
	re_bind_sock(listen_sock);		/*�����ذ�*/

	if (bind(listen_sock, (struct sockaddr *)&local, 
		sizeof(local)) == SOCKET_ERROR)
		return BUILD_ERROR(_OSerrno(), 0);		/*ע�⣺���˿ڱ�ռ��ʱ������Ҫ������������룬�û��Ż��������ü����˿�*/
	
	return listen_sock;
}

int 
setnonblocking( int s)
{
	int				rc;
	unsigned long	arg = 1;

	/**
	 *	���÷�������ʽ����
	 */
	rc = ioctlsocket(s, FIONBIO, (unsigned long*)&arg);
	if(rc == SOCKET_ERROR)
		return -1;

	return 0;
}

/**
 *	send_nonblock		- ���������͹̶���С����
 *
 *	@sockfd			[in]�׽���
 *	@buf			[in]�������ݻ���
 *	@size			[in]�������ݴ�С
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬error.h
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
 *	recv_data		- ���������չ̶���С���ݣ��ڳ�ʱʱ��(timeout)�ڲ������겻����
 *
 *	@sockfd			[in]	�׽���
 *	@buf			[in]	�������ݻ���
 *	@size			[in]	�������ݴ�С
 *	@timeout		[in]	���յĳ�ʱʱ��(��)��Ĭ��Ϊ3
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬error.h�Ĵ�����
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
 *	selectSocket		- ���������״̬
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
	//  ����SO_REUSEADDRѡ��(��������������)
	optval   =   0x1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, 4);
}
	