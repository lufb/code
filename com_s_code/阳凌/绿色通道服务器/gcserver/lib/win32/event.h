/*
 *	lib/win32/event.h
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

#include	"features.h"

#ifndef	__EVENT_HEADER__
#define __EVENT_HEADER__

struct futex_event {
#ifdef WIN32
	HANDLE				__cond;
#else
	sem_t				__sem;
#endif
};

/**
 *	futex_event_init - 初始化一个事件
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
futex_event_init(struct futex_event *event);

/**
 *	futex_event_destroy - 销毁一个事件
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
futex_event_destroy(struct futex_event *event);

/**
 *	futex_event_post - 投递一个事件信号
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
futex_event_post(struct futex_event *event);

/**
 *	futex_event_wait - 等待一个事件的信号
 *
 *	return:
 *		0		成功
 *		!0		失败
 */
int
futex_event_wait(struct futex_event *event);

/**
 *	futex_event_waitdwait - 等待一个事件的信号(超时控制)
 *
 *	return:
 *		==	0		超时
 *		>	0		成功
 *		<	0		失败
 */
int
futex_event_timedwait(struct futex_event *event, unsigned int msec);

#endif	/*	__EVENT_HEADER__*/

