#ifndef _SELECT_MAIN_H_
#define _SELECT_MAIN_H_

#include "http.h"

#define MAX_THREAD				256				/** ����߳���*/
#define MAX_IOBUFFER_LEN		102400			/** ��󻺳�����С(bytes)*/
#define	SELECT_TIMEOUT			5				/** select�ĳ�ʱʱ��(��)*/
#define SOCKET_BUFF_SIZE		64*1024			/** socket�Ļ�������С(bytes) 64k */


extern bool Global_Run;

int dealClient(SOCKET cliSock, SOCKET srvSock);

/** ����һ��timeval��ʱ��*/
#define INIT_TIMEVAL(_timeval_, _timeout_)		\
	do{											\
		_timeval_.tv_sec = _timeout_;			\
		_timeval_.tv_usec = 0;					\
	}while(0)									\

/** ���绯һ��fd_set*/
#define INIT_SELECT(_fd_, _sock1_, _sock2_)		\
	do{											\
		FD_ZERO(&_fd_);							\
		FD_SET(_sock1_, &_fd_);					\
		FD_SET(_sock2_, &_fd_);					\
	}while(0)									\

 /** �ж��û��Ƿ��Ѿ�ֹͣ�߳�*/
#define USER_CANCLE (Global_Run == false)

/** �ж��Ƿ�Ϊ�ͻ���*/
#define IS_CLI(_hs_) ((_hs_.towards) == 0)

/** �ж��Ƿ�Ϊ�����*/
#define IS_SRV(_hs_) ((_hs_.towards) == 1)

/** �ô��������ж��Ƿ�Ҫ���ٹر�*/
#define	NEED_FAST_CLOSE(_err_) \
	((((_err_) & 0x7FFF0000) >> 16) == (EFASTCLOSE))

typedef struct _THREAD_PARAM
{
	SOCKET				cliSock;
	char				DstIP[256];
	unsigned short		DstPort;
}THREAD_PARAM;



/** �����׽��ֵ������Ϣ*/
typedef struct _SOCK_INFO_
{
	SOCKET				sock;						/** �Լ���socket�׽���*/
	SOCKET				peerSock;					/** �Եȵ�socket�׽���*/
	unsigned int		towards:1;					/*** 0:cliSock 1:srvSock*/
	unsigned int		padding:31;					/** ����*/
	/** ��������С,+4������Ҫ��"\r\n\r\n"*/
	char				buffer[MAX_HTTPHEAD_SIZE+MAX_IOBUFFER_LEN+4];
	unsigned short		usUnitSerial;				/** ��װ����Ҫ�õ�unit*/
	unsigned short		pad;						/** ����*/			
	unsigned long		ulLinkNo;					/** ��װ����Ҫ�õ���link*/
	HTTP_PARASE_ENG		engine;						/** http��������*/
	
}SOCK_INFO;




#endif