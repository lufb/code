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
#ifndef _ARGO_SOCKET_H_
#define _ARGO_SOCKET_H_

#include <windows.h>

class MSocket
{
public:
								MSocket();
								~MSocket();
								
	static int					initNetWork();
	static void					destroy(void);
	static void					cleanup();
	static int					setnonblocking( int s);
	static int					sendDataNonBlock( int sockfd, char* buf, int size );
	static int					selectSocket( IN const int* pfd, IN int* pflag, IN int count, IN int sec, IN int usec );
	static int					rcvDataNonBlock( int sockfd, char* buf, int size, int timeout = 5);
	static int					dnsToIP(char* strDns, size_t size, char* dst, size_t dstSize);
	static int 					block_connect(const char *ip, unsigned short port);
	static void					close_sock(int s);
};



#endif