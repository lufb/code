/**
 *	include/bits/gcs_errno.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__ERRNO_HEADER__
#define __ERRNO_HEADER__

#define	GCEOS				1			/*	操作系统错误*/
#define	GCEABORT			2			/*	断言错误*/
#define	GCENOMEM			3			/*	内存不足*/
#define	GCEINVAL			4			/*	参数错误*/
#define GCENORES			5			/*	资源不足*/
#define	GCENOBUFS			6			/*	缓冲区不足*/
#define	GCELINKNO			7			/*	过期的LINKNO*/
#define	GCEQFULL			8			/*	队列满*/
#define	GCEPENDING			9			/*	操作已经挂起*/
#define	GCEPARSE			10			/*	解析错误*/
#define	GCEMISSING_IP		11			/*	丢失IP错误*/
#define	GCEMISSING_PORT		12			/*	丢失PORT错误*/
#define	GCEMISSING_CHANNO	13			/*	丢失UNITNO错误*/
#define	GCEMISSING_UNITNO	14			/*	丢失UNITNO错误*/
#define	GCEMISSING_TUNNEL	15			/*	丢失TUNNEL错误*/
#define	GCEMISSING_LENGTH	16			/*	丢失Length错误*/
#define	GCEMISSING_GCTYPE	17			/*	丢失GcType错误*/
#define	GCEMISSING_EBLANK	18			/*	丢失BODY空行错误*/
#define	GCELOGIC			19			/*	逻辑错误*/
#define	GCEDNS				20			/*	DNS错误*/

#ifdef WIN32
#define	_OSerrno()			GetLastError()
#else
#include <errno.h>
#define	_OSerrno()			errno
#endif

#define gen_errno(_sys_, _sp_) \
	(((_sys_) & 0xFFFF | (((_sp_) << 16) & 0xFFFF0000)) | 0x80008000)

#endif	/*	__ERRNO_HEADER*/