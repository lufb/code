/************************************************************************/
/* �ļ���:                                                              */
/*			base/MSocket.h												*/
/* ����:																*/
/*			����һЩ�й�socket�Ļ��������ӿ�							*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-25	¬����		����								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_SOCKET_H_
#define _M_SOCKET_H_

#include <windows.h>

class MSocket
{
public:
								MSocket();
								~MSocket();
								
	static int					initNetWork();
	static void					destroy(void);
	static void					cleanup();
	static int					fastClose(SOCKET s);
	static int					close(SOCKET s);
	static int					setSockBuf(SOCKET sock, size_t sockBufSize);
	static int					getsockError( SOCKET s);
	static int					setnonblocking( SOCKET s);
	static int					setBlocking(SOCKET s);
	static int					sendDataNonBlock( int sockfd, char* buf, int size );
	static int					selectSocket( IN const int* pfd, IN int* pflag, IN int count, IN int sec, IN int usec );
	static int					rcvDataNonBlock( int sockfd, char* buf, int size, int timeout = 2);
	static int					asynConnect( unsigned int ip, unsigned short port,  unsigned int *s);
	static int					dnsToIP(char* strDns, size_t size, char* dst, size_t dstSize);
};



#endif