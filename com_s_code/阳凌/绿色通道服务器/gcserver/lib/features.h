/*
 *	include/features.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	定义不同系统与编译环境下的特征值
 *
 *	注意:
 *		这个头文件是所有头文件的根节点,
 *	在这里会配置预编译的初始化宏.
 *
 *					--	阳凌
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__FEATURES_HEADER__
#define	__FEATURES_HEADER__

#include <assert.h>

#ifdef WITH_WARNING_LEVEL4_COMPILE
#define	RPC_NO_WINDOWS_H
#pragma warning (disable: 4127)
#endif

#ifdef WIN32
	#ifndef	INLINE_FN
		#define	INLINE_FN	__forceinline
	#endif
	/**	
	 *	#define _WIN32_WINNT 0x500 宏
	 *	lock.h中的SignalObjectAndWait要求,必须在include <windows.h>之前定义
	 */
	#define _WIN32_WINNT 0x500
#include	<windows.h>
#else
	/*	Linux Or Other OS 在这里添加一些代码*/
	#ifndef INLINE_FN
		#define INLINE_FN	__inline__
	#endif
#include	<pthread.h>
#endif

#if !defined(_U_) && defined(__GNUC__)
#define _U_	__attribute__((unused))
#endif

#ifndef _U_
#define _U_
#endif

#define IN
#define OUT
#endif

#ifndef	WIN32
#define UNREFERENCED_PARAMETER(P)          \
    /*lint -e527 -e530 */ \
    { \
        (P) = (P); \
    } \
#endif

#endif	/*	__FEATURES_HEADER__*/
