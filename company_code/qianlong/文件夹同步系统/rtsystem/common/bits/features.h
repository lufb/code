/*
 *	include/features.h
 *
 *	Copyright (C) 2012 阳凌 <yl.tienon@gmail.com>
 *
 *	定义与系统相关的一些兼容数据结构与数据类型
 *
 *	请开发者注意:这个头文件是所有头文件的根节点
 *	在这里会配置预编译的初始化宏.如果阁下非要直接
 *	引用这个头文件,为了避免一些警告的信息提示,
 *	请把本文件务必放在第一个include的位置.
 *
 *					--	阳凌
 *
 *	修改历史:
 *
 *	2012-09-16 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__FEATURES_HEADER__
#define	__FEATURES_HEADER__

#ifdef _DEBUG
#include <assert.h>
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

#endif	/*	__FEATURES_HEADER__*/