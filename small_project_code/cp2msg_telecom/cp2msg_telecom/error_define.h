#ifndef _ERROR_DEFINE_H_
#define _ERROR_DEFINE_H_
#include <stdio.h>
/*
	定义相应的错误码
*/
#define		E_SUCESS			0			/* 正常，没出错		*/
#define		E_TIMEOUT			1			/* 收数据超时		*/
#define		E_RCV				2			/* 收数据出错		*/
#define		E_MYABORT			3			/* 应用层断言出错	*/
#define		E_LINK_TIMEOUT		4			/* 链路超时			*/
#define		E_FULL_QUE			5			/* 队列已满			*/
#define		E_ERR_PRO			6			/* 未知协议号		*/
#define		E_ERR_SIZE			7			/* 协议数据大小不正确*/




#ifdef WIN32
#define	_OSerrno()			GetLastError()
#else
#include <errno.h>
#define	_OSerrno()			errno
#endif

/* 组装错误码:高16位为用户错误码，低16位为系统错误码 */
/* 0x80008000是为了保证组装的错误码必定是个负数 */
/* 要获相应错误码方法：用相应16位与7FFF相'&'就得到相应错误码 */
#define BUILD_ERROR(_sys_, _sp_) \
(((_sys_) & 0xFFFF | (((_sp_) << 16) & 0xFFFF0000)) | 0x80008000)

/* 用组装的错误码得到操作系统的错误码 */
#define GET_SYS_ERR(_errCode_)		\
((_errCode_) & 0x7FFF)

/* 用组装的错误码得到用户的错误码 */
#define GET_USER_ERR(_errCode_)		\
	(((_errCode_) & 0x7FFF0000) >> 16)

#define PRINT_ERR_NO(_errcode_)				\
	printf("syserror[%d] usrerror[%d]\n",	\
	GET_SYS_ERR(_errcode_),					\
	GET_USER_ERR(_errcode_))



#endif
