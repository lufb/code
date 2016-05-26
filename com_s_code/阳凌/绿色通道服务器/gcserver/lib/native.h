/*
 *	include/native.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	������һЩ������صĻ������ܺ���
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef		__NATIVE_HEADER__
#define		__NATIVE_HEADER__

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>
#include	"features.h"


#define		IS_NULL(_ptr_)			((void *)((_ptr_) == NULL))
#define		IS_NOT_NULL(_ptr_)		((void *)((_ptr_) != NULL))

#define		IS_SET_ID(_base, _key) \
	(*((unsigned int *)(_base) + ((_key) >> 5)) & (1 << ((_key) & 0x1f)))

#define		SET_ID(_base, _key) \
	do {*((unsigned int *)(_base) + ((_key) >> 5)) |= \
		(1 << ((_key) & 0x1f));} while(0)

#define		UNSET_ID(_base, _key) \
	do {*((unsigned int *)(_base) + ((_key) >> 5)) &= \
			~(1 << ((_key) & 0x1f));} while(0)

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 *	zmalloc - �������ݲ���0
 *
 *	@size:		��Ҫ������ڴ��С
 *
 *	return:
 *		NULL	ʧ��
 *		!NULL	�ɹ�
 */
static INLINE_FN void *zmalloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if(ptr)
		memset(ptr, 0, size);

	return ptr;
}

/**
 *	__builtin_is_timeout - ��ʱ�ж�
 */
static INLINE_FN int __builtin_is_timeout(
	time_t _std, time_t a, unsigned int timeout)
{
	if((int)(_std - a) > (int)timeout)
		return 1;
	else
		return 0;
}

/**
 *	noarch_sleep - ��ϵͳ������˯��
 *
 *	@msec:		����
 *
 *	return
 *		��
 */
static INLINE_FN void noarch_sleep(unsigned int msec)
{
#ifdef WIN32
	Sleep(msec);
#else
	long long _a;
	struct timeval tv;

	_a = msec * 1000;
	tv.tv_sec = _a / 1000000;
	tv.tv_usec = _a % 1000000;
	return select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &tv);
#endif
}

#ifdef __cplusplus
}
#endif

#endif		/*	__NATIVE_HEADER__*/