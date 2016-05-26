/*
 *	lib/win32/posix.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	������һЩ��POSIX��׼��صĺ���ʵ��
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
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
 *	futex_event_init - ��ʼ��һ���¼�
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
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
 *	futex_event_destroy - ����һ���¼�
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
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
 *	futex_event_post - Ͷ��һ���¼��ź�
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
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
 *	futex_event_wait - �ȴ�һ���¼����ź�
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
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
 *	futex_event_waitdwait - �ȴ�һ���¼����ź�(��ʱ����)
 *
 *	return:
 *		==	0		��ʱ
 *		>	0		�ɹ�
 *		<	0		ʧ��
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
 *	pthread_create - ����һ���߳�
 *
 *	@thread:	�߳̾��
 *	@attr:		�߳�����(unused) ignore
 *			compatibility with pthread
 *	@start_routine:
 *				�̺߳���
 *	@arg:		�̲߳���
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
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
