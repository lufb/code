/*
 *	lib/win32/pthread.h
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
 *	pthread_mutex_init - ��ʼ��һ��������
 *
 *	@mutex:		������
 *	@attr:		������(unused) ignore
 *			compatibility with pthread
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
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
 *	pthread_mutex_destroy - ����һ��������
 *
 *	@mutex:		������
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
 */
static INLINE_FN int
pthread_mutex_destroy(
	IN OUT pthread_mutex_t *mutex)
{
	DeleteCriticalSection(&(mutex->cs));
	return 0;
}

/**
 *	pthread_mutex_lock - ����
 *
 *	@mutex:		������
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
 */
static INLINE_FN int
pthread_mutex_lock(
	IN OUT pthread_mutex_t *mutex)
{
	EnterCriticalSection(&(mutex->cs));
	return 0;
}

/**
 *	pthread_mutex_unlock - ����
 *
 *	@mutex:		������
 *
 *	return:
 *		0		�ɹ�
 *		!0		ʧ��
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
	IN	void *arg);

#ifdef __cplusplus
}
#endif

#endif		/*	__PTHREAD_HEADER__*/