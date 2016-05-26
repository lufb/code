#include <winsock2.h>
#include <assert.h>
#include "MSocket.h"
#include "error.h"


/**
 *	InitNetWork -		���绯���绷��
 *	
 *	return
 *		==0			����
 *		!=0			����
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
 *	cleanup - �������绷��
 *
 *	return
 *		��
 */
void MSocket::cleanup()
{
	WSACleanup();
}


/**
 *	fastClose	-		���ٹر�SOCKET RST��
 *	
 *	@s			[in]	��Ҫ�رյ��׽���
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
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
 *	close		 -		�ر�socket
 *	
 *	@s			[in]	��Ҫ�رյ��׽���
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
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
 *	setSockBuf	-	�����׽��ֵķ��ͻ���������ջ�����
 *
 *	@sock:		[in]	��Ҫ���õ��׽���
 *	@sockBufSize:[in]	��������С
 *	return
 *				==0		�ɹ�
 *				< 0	ʧ��
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
 *	ne_getsock_errno - ����SOCKET������,�õ�ָ��SOCKET�����һ�δ�����
 *
 *	@sock:		[in]	�׽���
 *
 *	return
 *		>	0	�������
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
 *	setnonblocking - ����SOCKET������
 *
 *	@s:		[in]	SOCKET������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 *
 */
int MSocket::setnonblocking( SOCKET s)
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
 *	setBlocking - ����SOCKET����
 *
 *	@s:		[in]	SOCKET������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 *
 */
int MSocket::setBlocking(SOCKET s)
{
	int				rc;
	unsigned long	arg = 0;

	/**
	 *	���÷�������ʽ����
	 */
	rc = ioctlsocket(s, FIONBIO, (unsigned long*)&arg);
	if(rc == SOCKET_ERROR)
		return -1;

	return 0;
}

/**
 *	sendData		- ���������͹̶���С����
 *
 *	@sockfd			[in]�׽���
 *	@buf			[in]�������ݻ���
 *	@size			[in]�������ݴ�С
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬error.h
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
 *	asynConnect - �첽����
 *
 *	@ip:				IP��ַ(big endian)
 *	@port:				PORT�˿�(big endian)
 *	@s:					�����õ�SOCKET������
 *
 *	return
 *		>	0			�ɹ�(��Ҫ����ͨ��select��̽��SOCKET�Ƿ��Ѿ���д)
 *		==	0			�ɹ�(����Ҫͨ��select��̽��)
 *		<	0			ʧ��(remark)
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
	 *	����һ��SOCKET������
	 */
	_s = socket(AF_INET, SOCK_STREAM, 0);
	if(_s == INVALID_SOCKET)
		return -1;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = port;

	/**
	 *	���÷�������ʽ����
	 */
	rc = setnonblocking(_s);
	if(rc) {
		close(_s);
		return -2;
	}

	/**
	 *	���ӷ�����
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
 *	dnsToIP -			������ת��ΪIP��ַ
 *
 *	@strDns:			����Ĵ�ת����dns����(������IP��ַ)
 *	@size:				������ַ�������
 *	@dst:				��Ž���ĵ�ַ
 *	@dstSize:			��Ž���Ŀռ䳤��
 *
 *	return
 *		==	0			�ɹ�
 *		<	0			ʧ��(remark)
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
			//��Ч������
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
		//��������IP��ַ
		strncpy(dst, strDns, dstSize);
	}
	
	return 0;
}

/**
 *	setSock		-			�����׽��ֵ�����(����������������ջ�������С)
 *
 *	@sock			[in]		��Ҫ���õ��׽���
 *	@bufSize		[in]		��������С
 *
 *	return
 *								��
 */
void MSocket::setSock(SOCKET sock, size_t bufSize)
{
	assert(sock != INVALID_SOCKET);

	setnonblocking(sock);
	setSockBuf(sock, bufSize);
}

