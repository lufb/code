/*
 *	include/features.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	���岻ͬϵͳ����뻷���µ�����ֵ
 *
 *	ע��:
 *		���ͷ�ļ�������ͷ�ļ��ĸ��ڵ�,
 *	�����������Ԥ����ĳ�ʼ����.
 *
 *					--	����
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
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
	 *	#define _WIN32_WINNT 0x500 ��
	 *	lock.h�е�SignalObjectAndWaitҪ��,������include <windows.h>֮ǰ����
	 */
	#define _WIN32_WINNT 0x500
#include	<windows.h>
#else
	/*	Linux Or Other OS ���������һЩ����*/
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
