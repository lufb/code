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
 *	InitNetWork -		���绯���绷��
 *	
 *	return
 *		==0			����
 *		!=0			����
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
 *	cleanup - �������绷��
 *
 *	return
 *		��
 */
void MSocket::cleanup()
{
#ifndef LINUXCODE
	WSACleanup();
#endif
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
 *	dnsToIP -			������ת��ΪIP��ַ
 *
 *	@strDns:			����Ĵ�ת����dns����(������IP��ַ)
 *	@size:				������ַ�������
 *	@dst:				��Ž���ĵ�ַ
 *	@dstSize:			��Ž���Ŀռ䳤��
 *
 *	return
 *		==	0			�ɹ�(����Ҫͨ��select��̽��)
 *		<	0			ʧ��(remark)
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
			//��Ч������
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
		//��������IP��ַ
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

