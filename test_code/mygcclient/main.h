#ifndef _SELECT_MAIN_H_
#define _SELECT_MAIN_H_

#include "http.h"

#define MAX_THREAD				256				/** 最大线程数*/
#define MAX_IOBUFFER_LEN		102400			/** 最大缓冲区大小(bytes)*/
#define	SELECT_TIMEOUT			5				/** select的超时时间(秒)*/
#define SOCKET_BUFF_SIZE		64*1024			/** socket的缓冲区大小(bytes) 64k */


extern bool Global_Run;

int dealClient(SOCKET cliSock, SOCKET srvSock);

/** 设置一个timeval的时间*/
#define INIT_TIMEVAL(_timeval_, _timeout_)		\
	do{											\
		_timeval_.tv_sec = _timeout_;			\
		_timeval_.tv_usec = 0;					\
	}while(0)									\

/** 初如化一个fd_set*/
#define INIT_SELECT(_fd_, _sock1_, _sock2_)		\
	do{											\
		FD_ZERO(&_fd_);							\
		FD_SET(_sock1_, &_fd_);					\
		FD_SET(_sock2_, &_fd_);					\
	}while(0)									\

 /** 判断用户是否已经停止线程*/
#define USER_CANCLE (Global_Run == false)

/** 判断是否为客户端*/
#define IS_CLI(_hs_) ((_hs_.towards) == 0)

/** 判断是否为服务端*/
#define IS_SRV(_hs_) ((_hs_.towards) == 1)

/** 用错误码来判断是否要快速关闭*/
#define	NEED_FAST_CLOSE(_err_) \
	((((_err_) & 0x7FFF0000) >> 16) == (EFASTCLOSE))

typedef struct _THREAD_PARAM
{
	SOCKET				cliSock;
	char				DstIP[256];
	unsigned short		DstPort;
}THREAD_PARAM;



/** 描述套接字的相关信息*/
typedef struct _SOCK_INFO_
{
	SOCKET				sock;						/** 自己的socket套接字*/
	SOCKET				peerSock;					/** 对等的socket套接字*/
	unsigned int		towards:1;					/*** 0:cliSock 1:srvSock*/
	unsigned int		padding:31;					/** 保留*/
	/** 缓冲区大小,+4是由于要加"\r\n\r\n"*/
	char				buffer[MAX_HTTPHEAD_SIZE+MAX_IOBUFFER_LEN+4];
	unsigned short		usUnitSerial;				/** 组装数据要用的unit*/
	unsigned short		pad;						/** 保留*/			
	unsigned long		ulLinkNo;					/** 组装数据要用到的link*/
	HTTP_PARASE_ENG		engine;						/** http解析引擎*/
	
}SOCK_INFO;




#endif