/************************************************************************/
/* 文件名:                                                              */
/*			base/MSocket.h												*/
/* 功能:																*/
/*			定义一些有关socket的基本操作接口							*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/
#ifndef _ARGO_SOCKET_H_
#define _ARGO_SOCKET_H_

#include <stdlib.h>
#ifndef LINUXCODE
#include <windows.h>
#else
#define IN
#endif

class MSocket
{
public:
								MSocket();
								~MSocket();
#ifndef LINUXCODE								
	static int					initNetWork();
#endif
	static void					destroy(void);
	static void					cleanup();
	static int					sendDataNonBlock( int sockfd, char* buf, int size );
	static int					selectSocket( IN const int* pfd, IN int* pflag, IN int count, IN int sec, IN int usec );
	static int					rcvDataNonBlock( int sockfd, char* buf, int size, int timeout = 5);
	static int					dnsToIP(char* strDns, size_t size, char* dst, size_t dstSize);
	static int 					block_connect(const char *ip, unsigned short port);
	static void					close_sock(int s);
};



#endif