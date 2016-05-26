/*
 *	include/native.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	定义了一些本地相关的基础功能函数
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
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
 *	zmalloc - 分配数据并填0
 *
 *	@size:		需要分配的内存大小
 *
 *	return:
 *		NULL	失败
 *		!NULL	成功
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
 *	__builtin_is_timeout - 超时判断
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
 *	noarch_sleep - 无系统依赖的睡眠
 *
 *	@msec:		毫秒
 *
 *	return
 *		无
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