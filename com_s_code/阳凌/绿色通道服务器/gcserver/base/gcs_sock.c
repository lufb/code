/**
 *	base/gcs_socket.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#include <stdio.h>
#include "features.h"
#include "gcs_sock.h"
#include "gcs_errno.h"
#include "gcs_main.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: base/gcs_sock.c,"
	"v 1.00 2013/10/29 10:56:40 yangling Exp $ (LBL)";
#endif

/**
 *	ne_startup - ��ʼ�����绷��
 *
 *	return
 *		��
 */
void ne_startup()
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD(2, 2);
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if(err != 0)
		return;	
	if(LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2 ) {
		WSACleanup();
		return; 
	}
#endif
}

/**
 *	ne_cleanup - �������绷��
 *
 *	return
 *		��
 */
void ne_cleanup()
{
#ifdef WIN32
	WSACleanup();
#endif
}

/**
 *	ne_setreuseaddr - ���õ�ַ����
 *
 *	@s:			SOCKET������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
ne_setreuseaddr(IN int s)
{
	int		rc;
	BOOL	val = 1;

	rc = setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&val, sizeof(val));
	if(rc)
		return -1;
	else
		return 0;
}

/**
 *	ne_close - �ر�SOCKET
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 *
 *	remark:
 *		a specific error code can be retrieved
 *	by calling GetLastError
 */
int
ne_close(IN int s)
{
	if(s == INVALID_SOCKET)
		return 0;

#ifdef WIN32
	return closesocket(s);
#else
	return close(s);
#endif
}

/**
 *	ne_fast_close - ���ٹر�SOCKET RST��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 *
 *	remark:
 *		a specific error code can be retrieved
 *	by calling GetLastError
 */
int
ne_fast_close(IN int s)
{
	struct linger opt;

	if(s == INVALID_SOCKET)
		return 0;

	opt.l_onoff	= 1;
	opt.l_linger= 0;

	setsockopt(s, SOL_SOCKET, SO_LINGER,
			(const char*)&opt, sizeof(opt));
#ifdef WIN32
	return closesocket(s);
#else
	return close(s);
#endif
}

/**
 *	ne_getsock_errno - ����SOCKET������,�õ�ָ��SOCKET�����һ�δ�����
 *
 *	@s:		SOCKET������
 *
 *	return
 *		>	0	�������
 *		<	0	remark
 *
 *	remark:
 *		a specific error code can be retrieved
 *	by calling GetLastError
 */
int
ne_getsock_errno(IN int s)
{
	int errorcode = -2;
	int len = sizeof(int);

	if(!getsockopt(s, SOL_SOCKET, SO_ERROR, (char *)&errorcode, (int *)&len))
		return errorcode;
	else
		return -1;
}

/**
 *	ne_setnonblocking - ����SOCKET������
 *
 *	@s:		SOCKET������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 *
 *	remark:
 *		a specific error code can be retrieved
 *	by calling GetLastError
 */
int
ne_setnonblocking(IN int s)
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
 *	ne_setblocking - ����SOCKET����
 *
 *	@s:		SOCKET������
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 *
 *	remark:
 *		a specific error code can be retrieved
 *	by calling GetLastError
 */
int
ne_setblocking(IN int s)
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
 *	ne_inet_ntopx - �̰߳�ȫ��IP��ַ�Ͷ˿�ת�����ַ���
 *
 *	@ip:			IP��ַ(big endian)
 *	@port:			�˿�(big endian)
 *	@addr:			��ַ������
 *	@addr_len:		��ַ����������
 *
 *	return
 *		��ַ��Ϣ
 */
char*
ne_inet_ntopx(IN unsigned int ip,
	IN unsigned short port, OUT char *addr, IN size_t addr_len)
{
	memset(addr, 0, addr_len);

	ip = htonl(ip);

	sprintf(addr, "%d.%d.%d.%d:%d",
		_IPV4_A_(ip), _IPV4_B_(ip),
		_IPV4_C_(ip), _IPV4_D_(ip), ntohs(port));

	return addr;
}

/**
 *	ne_connect2 - ���ӷ�����
 *
 *	@ip:				IP��ַ(big endian)
 *	@port:				PORT�˿�(big endian)
 *
 *	return:
 *		> 0		�ɹ�
 *		<=0		ʧ��
 */
int ne_connect2(
	IN unsigned int ip, IN unsigned short port)
{
	int	s;
	struct sockaddr_in addr;
	
	if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = port;
	
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

/**
 *	ne_asyn_connect - �첽����
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
int
ne_asyn_connect(IN unsigned int ip,
	IN unsigned short port, IN int *s)
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
	rc = ne_setnonblocking(_s);
	if(rc) {
		ne_close(_s);
		return -1;
	}

	/**
	 *	���ӷ�����
	 */
	if(connect(_s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		if(WSAEWOULDBLOCK == GetLastError()) {
			*s = _s;
			return 1;
		} else {
			ne_close(_s);
			return -1;
		}
	} else {
		*s = _s;
		return 0;
	}
}


