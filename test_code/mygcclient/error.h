/**
 *	errno.h
 *	
 *	Copyright (C) 2013 卢富波 <1164830775@qq.com>
 *
 *
 *	修改历史:
 *
 *	2013-11-14 - 首次创建
 *
 *                     卢富波  <1164830775@qq.com>
 */
#ifndef _ERROR_H_
#define _ERROR_H_

#define		ESUCESS			0			/** 正常，没出错*/
#define		EOS				1			/**	操作系统错误*/
#define		EABORT			2			/** 断言出错*/			
#define		ELOGIC			3			/** 程序逻辑出错*/
#define		ECCLOSE			4			/** 客户端套接字关闭*/
#define		ESCLOSE			5			/** 服务端套接字关闭*/
#define		ESETSOCK		6			/** 设置套接字属性出错*/
#define		EFASTCLOSE		7			/** 需求快速关闭*/
#define		EBUFLESS		8			/** 缓冲区过小*/
#define		EENGSTATUS		9			/** 引擎状态出错*/

#ifdef WIN32
#define	_OSerrno()			GetLastError()
#else
#include <errno.h>
#define	_OSerrno()			errno
#endif

/** 组装错误码:高16位为用户错误码，低16位为系统错误码*/
/** 0x80008000是为了保证组装的错误码必定是个负数*/
/** 要获相应错误码方法：用相应16位与7FFF相'&'就得到相应错误码*/
#define BUILD_ERROR(_sys_, _sp_) \
	(((_sys_) & 0xFFFF | (((_sp_) << 16) & 0xFFFF0000)) | 0x80008000)

/** 用组装的错误码得到操作系统的错误码*/
#define GET_SYS_ERR(_errCode_)		\
	((_errCode_) & 0x7FFF)

/** 用组装的错误码得到用户的错误码*/
#define GET_USER_ERR(_errCode_)		\
	(((_errCode_) & 0x7FFF0000) >> 16)

#endif