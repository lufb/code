/*
 *	include/features.h
 *
 *	Copyright (C) 2012 ���� <yl.tienon@gmail.com>
 *
 *	������ϵͳ��ص�һЩ�������ݽṹ����������
 *
 *	�뿪����ע��:���ͷ�ļ�������ͷ�ļ��ĸ��ڵ�
 *	�����������Ԥ����ĳ�ʼ����.������·�Ҫֱ��
 *	�������ͷ�ļ�,Ϊ�˱���һЩ�������Ϣ��ʾ,
 *	��ѱ��ļ���ط��ڵ�һ��include��λ��.
 *
 *					--	����
 *
 *	�޸���ʷ:
 *
 *	2012-09-16 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
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

#endif	/*	__FEATURES_HEADER__*/