/*
 *	lib/win32/posix.c
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	定义了一些与POSIX标准相关的函数实现
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#include <process.h>
#include "event.h"
#include "pthread.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: lib/win32/posix.c,"
	"v 1.00 2013/10/29 12:15:40 yangling Exp $ (LBL)";
#endif

/**
 *	futex_event_init - 初始化一个事件
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
futex_event_init(struct futex_event *event)
{
#ifdef WIN32
	event->__cond = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(!event->__cond)
		return -1;
	else
		return 0;
#else
	if(sem_init(&(event->__sem), 0, 0))
		return -1;
	else
		return 0;
#endif
}

/**
 *	futex_event_destroy - 销毁一个事件
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
futex_event_destroy(struct futex_event *event)
{
	 
#ifdef WIN32
	if(CloseHandle(event->__cond)) {
		event->__cond = NULL;
		return 0;
	} else
		return -1;
#else
	if(sem_destroy(&(event->__sem)))
		return -1;
	else
		return 0;
#endif
}

/**
 *	futex_event_post - 投递一个事件信号
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
futex_event_post(struct futex_event *event)
{
#ifdef WIN32
	if(SetEvent(event->__cond))
		return 0;
	else
		return -1;
#else
	int rc;

	while((rc = sem_post(&(event->__sem))) && (errno == EINTR));

	return rc;
#endif
}

/**
 *	futex_event_wait - 等待一个事件的信号
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
futex_event_wait(struct futex_event *event)
{
#ifdef WIN32
	int rc;
	rc = WaitForSingleObject(event->__cond, INFINITE);
	if(rc == WAIT_OBJECT_0)
		return 0;
	else
		return -1;
#else
	int rc;

	while((rc = sem_post(&(event->__sem))) && (errno == EINTR));

	return rc;
#endif
}

/**
 *	futex_event_waitdwait - 等待一个事件的信号(超时控制)
 *
 *	return:
 *		==	0		超时
 *		>	0		成功
 *		<	0		失败
 */
int
futex_event_timedwait(struct futex_event *event, unsigned int msec)
{
	#ifdef WIN32
	int rc;
	rc = WaitForSingleObject(event->__cond, msec);
	if(rc == WAIT_OBJECT_0)
		return 1;
	else if(rc == WAIT_TIMEOUT)
		return 0;
	else
		return -1;
#else
	a
#endif
}

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
	IN	void *arg)
{
	UNREFERENCED_PARAMETER(attr);

	*thread = _beginthread(
		(void (__cdecl *)(void *))start_routine, 0, arg);
	if((long)*thread == -1)
		return -1;
	else
		return 0;
}
