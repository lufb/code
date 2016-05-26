/**
 *	include/bits/gcs_main.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_MAIN_HEADER__
#define __BITS_MAIN_HEADER__

#include	"features.h"

#define		GCSERVER_NAME		"gcserver"

#define	_IPV4_A_(_ipv4) \
((unsigned char)(((_ipv4) & 0xFF000000) >> 24))

#define	_IPV4_B_(_ipv4) \
((unsigned char)(((_ipv4) & 0x00FF0000) >> 16))

#define	_IPV4_C_(_ipv4) \
((unsigned char)(((_ipv4) & 0x0000FF00) >> 8))

#define	_IPV4_D_(_ipv4) \
	((unsigned char)(((_ipv4) & 0x000000FF) >> 0))

#define	GCS_THREAD_ZOMBIE		0		/*	��ʬ״̬*/
#define	GCS_THREAD_SLEEPING		1		/*	����˯��*/
#define	GCS_THREAD_RUNNING		2		/*	��������*/

#define	gcs_thread_sleep(_gth_)			\
	do {								\
		(_gth_)->status =				\
				GCS_THREAD_SLEEPING;	\
	} while(0)

#define	gcs_thread_wake(_gth_)			\
	do {								\
		(_gth_)->status =				\
				GCS_THREAD_RUNNING;		\
	} while(0)

struct gcs_thread {
	char			name[16];			/*	�߳�����*/
	unsigned int	status;				/*	�߳�����״̬*/
	HANDLE			handler;			/*	�߳̾��*/
	unsigned long	thread_id;			/*	�߳�ID*/
};

#endif	/*	__BITS_MAIN_HEADER__*/