/*
 *	include/local.h
 *
 *	Copyright (C) 2012 阳凌 <yl.tienon@gmail.com>
 *
 *	定义了一些本地相关的基础功能函数
 *
 *	修改历史:
 *
 *	2012-09-16 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef		__LOCAL_HEADER__
#define		__LOCAL_HEADER__

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>
#include	"features.h"


#define		IS_NULL(_ptr_)			((void *)((_ptr_) == NULL))
#define		IS_NOT_NULL(_ptr_)		((void *)((_ptr_) != NULL))

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 *	_zmalloc - 分配数据并填0
 *
 *	@size:		需要分配的内存大小
 *
 *	return:
 *		NULL	失败
 *		!NULL	成功
 */
static INLINE_FN void *_zmalloc(size_t size)
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

#ifdef __cplusplus
}
#endif

#endif		/*	__LOCAL_HEADER__*/