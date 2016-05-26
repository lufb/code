/*
 *	lib/win32/pthread.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	定义了一些线程同步相关的基础功能函数
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef		__PTHREAD_HEADER__
#define		__PTHREAD_HEADER__

#include	"features.h"

typedef struct {
	CRITICAL_SECTION	cs;
} pthread_mutex_t;

typedef struct {
	void	*			dummy;
}pthread_mutexattr_t;

#ifdef  __cplusplus
extern  "C" {
#endif

/**
 *	pthread_mutex_init - 初始化一个互斥锁
 *
 *	@mutex:		锁对象
 *	@attr:		锁属性(unused) ignore
 *			compatibility with pthread
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
static INLINE_FN int
pthread_mutex_init(
	IN OUT pthread_mutex_t *mutex,
	IN const pthread_mutexattr_t *attr)
{
	UNREFERENCED_PARAMETER(attr);
	InitializeCriticalSection(&(mutex->cs));
	return 0;
}

/**
 *	pthread_mutex_destroy - 销毁一个互斥锁
 *
 *	@mutex:		锁对象
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
static INLINE_FN int
pthread_mutex_destroy(
	IN OUT pthread_mutex_t *mutex)
{
	DeleteCriticalSection(&(mutex->cs));
	return 0;
}

/**
 *	pthread_mutex_lock - 加锁
 *
 *	@mutex:		锁对象
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
static INLINE_FN int
pthread_mutex_lock(
	IN OUT pthread_mutex_t *mutex)
{
	EnterCriticalSection(&(mutex->cs));
	return 0;
}

/**
 *	pthread_mutex_unlock - 解锁
 *
 *	@mutex:		锁对象
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
static INLINE_FN int
pthread_mutex_unlock(
	IN OUT pthread_mutex_t *mutex)
{
	LeaveCriticalSection(&(mutex->cs));
	return 0;
}

typedef unsigned long pthread_t;

typedef struct {
	void	*	dummy;
}pthread_attr_t;

/**
 *	pthread_create - 创建一个线程
 *
 *	@thread:	线程句柄
 *	@attr:		线程属性(unused) ignore
 *			compatibility with pthread
 *	@start_routine:
 *				线程函数
 *	@arg:		线程参数
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
pthread_create(
	OUT pthread_t *thread,
	IN	const pthread_attr_t *attr,
	IN	void *(*start_routine)(void*),
	IN	void *arg);

#ifdef __cplusplus
}
#endif

#endif		/*	__PTHREAD_HEADER__*/