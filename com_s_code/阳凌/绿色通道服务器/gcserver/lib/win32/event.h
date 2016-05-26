/*
 *	lib/win32/event.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	������һЩ�߳�ͬ����صĻ������ܺ���
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
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
 *	futex_event_init - ��ʼ��һ���¼�
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
futex_event_init(struct futex_event *event);

/**
 *	futex_event_destroy - ����һ���¼�
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
futex_event_destroy(struct futex_event *event);

/**
 *	futex_event_post - Ͷ��һ���¼��ź�
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
futex_event_post(struct futex_event *event);

/**
 *	futex_event_wait - �ȴ�һ���¼����ź�
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
futex_event_wait(struct futex_event *event);

/**
 *	futex_event_waitdwait - �ȴ�һ���¼����ź�(��ʱ����)
 *
 *	return:
 *		==	0		��ʱ
 *		>	0		�ɹ�
 *		<	0		ʧ��
 */
int
futex_event_timedwait(struct futex_event *event, unsigned int msec);

#endif	/*	__EVENT_HEADER__*/

