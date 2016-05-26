#ifdef GCS_WITH_RWLOCK
/*
 *	lib/win32/rwlock.h
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

#ifndef		__RWLOCK_HEADER__
#define		__RWLOCK_HEADER__

#include	"features.h"

/**
 *	��д��
 */
#define	_RW_ETIMEOUT	-1
#define	_RW_GENERAL		-2

#define	WAIT_FOREVER	0xFFFFFFFF
	
#ifdef WIN32
#define	_NO_LOCK		0
#define	_RD_LOCK		1
#define	_WR_LOCK		2
struct futex_rwlock {
	HANDLE			mutex;				/*	������*/
	HANDLE			rd_event;			/*	���ȴ����*/
	HANDLE			wr_event;			/*	д�ȴ����*/

	volatile int	rd_counter;			/*	������*/
    volatile int	wr_counter;			/*	д����*/
    volatile int	lock_type;			/*	������*/
};
#else
#error "Not yet implementation"
#endif

#ifdef	WIN32
/**
 *	��Ҫ����̫��Ķ�д��,ÿ����д����ռ��3�����
 *	����ľ�������ص���ϵͳ������
 *	ÿ���������һ���ں˶���,��ʵ����Ч�����
 *	WIN�ṩ����������CriticalSection�ٽ�����˵
 *	ÿ���ں˶���Ĳ������ᵼ�²���ϵͳ����һ��
 *	�û�̬���ں�̬��ת������,(ѹ�����е�CPU�Ĵ���,IO�˿ڵ�, �����л�TSS��)
 *	���·���һ�����ж�, Inter ϰ�߽� �����������trap
 *	����������൱����ʱ���
 *	��I386/686ϵͳ�����,�����Ϊ���ں�̬���û�̬
 *	����ϵͳ���еĴ��붼�����ں�̬.���ǵĳ����������û�̬
 *	WIN�ṩ����������CriticalSection�ٽ���
 *	��������I386/686 ������CAS Compare And Swap�ķ�ʽ��ʵ�ֵ�
 *	������״̬�л�,�����ٶ��൱��
 *	Ҫ��m_hMutex��ʹ�����WIN�ṩ����������CriticalSection�ٽ�����������
 *	Ŀǰ����VC60�Ļ�����,ֻ��������.��VS2008���Ѿ���һ����������Ľ������
 *
 *	��ʱ����ʵ����ĺ�ϲ��LINUX,�����������,�ṩ��һ���û�̬��������.��ĿǰΪֹ
 *	MS��û����VC60���ṩ�û�̬��spinlock������
 *
 *								--	����
 */

/**
 *	���ر�ע��:SignalObjectAndWait������LINUX��pthread_cond_wait�������е�����
 *	���ǵĹ�ͬ��:��˯��֮ǰ����ԭ�ӵĲ���h_Mutex�Ľ���,Ȼ��ȴ�.
 *	LINUX��pthread_cond_wait ���� ���������ѹ�����
 *	�����¼���m_hMutex����ٽ�����������������Ĺ�����ԭ�ӵ�
 *	��WIN�����Ǻ��������
 *	��WIN�µ�SignalObjectAndWait�ĺ��������Ѻ�,�ǲ����ټ�m_hMutex���ٽ�����
 *	������WIN����,Ҫ�������Ѻ��ǲ����ٴ����ڲ��ı���������
 *	m_nRDNum, m_nWRNum, m_LockType 3��������
 *	���Գ���ı�д�߼������LINUX���������
 *	������ֲLINUX�µ�ʱ��,ע�����.����ֲLINUX��ʱ��
 *	���Կ���2����ֲ����
 *	1:ֱ��ʹ�� �⺯��.pthread_rwlock_rdlock �Ⱥ���
 *	2:�ο� UNIX/LINUX ��ʥ��<APUE2 �߼�UNIX�������(�ڶ���)>������ܵĶ�д���ľ���ʵ�ַ�ʽ,
 *	�Լ������ں���ʹ�õ�ʱ����ʲô�ط���Ҫע��ĵط�
 *	������ϸ�����˿⺯�������ʵ�ֵ�,�Լ��������mutex���߳�ͬ������pthread_signal�����ʵ�ֶ�д����
 *
 *									--	����
 */

/**
 *	futex_rwlock_init - ��ʼ����д��
 *
 *	@rwlck:		��д��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static INLINE_FN int futex_rwlock_init(struct futex_rwlock *rwlck)
{
	memset(rwlck, 0, sizeof(struct futex_rwlock));

	/* �������ڱ����ڲ����ݵĻ�����*/
	rwlck->mutex = CreateMutex(NULL, FALSE, NULL);
    /* ��������ͬ����������̵߳��¼����ֶ��¼���*/
    rwlck->rd_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    /* ��������ͬ����ռ�����̵߳��¼����Զ��¼���*/
    rwlck->wr_event = CreateEvent(NULL, FALSE, FALSE, NULL);

	return 0;
}

/**
 *	futex_rwlock_destroy - ���ٶ�д��
 *
 *	@rwlck:		��д��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static INLINE_FN int futex_rwlock_destroy(struct futex_rwlock *rwlck)
{
	CloseHandle(rwlck->mutex);
	CloseHandle(rwlck->rd_event);
	CloseHandle(rwlck->wr_event);

	memset(rwlck, 0, sizeof(struct futex_rwlock));

	return 0;
}

/**
 *	futex_rwlock_rdlock - ��ȡ����
 *
 *	@rwlck:		��д��
 *	@ms:		��ʱʱ��(��λ:����)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static INLINE_FN int futex_rwlock_rdlock(
			struct futex_rwlock *rwlck, unsigned int ms)
{
	unsigned int	rc;

    WaitForSingleObject(rwlck->mutex, INFINITE);
	rwlck->rd_counter++;
    if (rwlck->lock_type == _WR_LOCK) {
		/*	��ǰ��д��,��ȥ˯�ߵȴ�*/
        rc = SignalObjectAndWait(rwlck->mutex, rwlck->rd_event, ms, FALSE);
        if(rc == WAIT_OBJECT_0) {
            return 0;
        } else {
            if(rc == WAIT_TIMEOUT)
                return _RW_ETIMEOUT;
			else
				return _RW_GENERAL;
        }
    }
    rwlck->lock_type = _RD_LOCK;
    ReleaseMutex(rwlck->mutex);

    return 0;
}

/**
 *	futex_rwlock_rdunlock - �������
 *
 *	@rwlck:		��д��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static INLINE_FN int futex_rwlock_rdunlock(struct futex_rwlock *rwlck)
{
#ifdef _DEBUG
	assert(rwlck->lock_type == _RD_LOCK);
#else
	if(rwlck->lock_type != _RD_LOCK)
		return _RW_GENERAL;
#endif

    WaitForSingleObject(rwlck->mutex, INFINITE);
	rwlck->rd_counter--;
    if (rwlck->rd_counter == 0) {
        if (rwlck->wr_counter  > 0) {
            /*	����һ����ռ�����߳�*/
            rwlck->lock_type = _WR_LOCK;
            SetEvent(rwlck->wr_event);
        } else {
            /*	û�еȴ��߳�*/
            rwlck->lock_type = _NO_LOCK;
        }
    }
    ReleaseMutex(rwlck->mutex);

	return 0;
}

/**
 *	futex_rwlock_wrlock - ��ȡд��
 *
 *	@rwlck:		��д��
 *	@ms:		��ʱʱ��(��λ:����)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static INLINE_FN int futex_rwlock_wrlock(
		struct futex_rwlock *rwlck, unsigned int ms)
{
	unsigned int rc;

    WaitForSingleObject(rwlck->mutex, INFINITE);
	rwlck->wr_counter++;
    if (rwlck->lock_type != _NO_LOCK) {
		/*	��ǰ�Ѿ����˳�����,������д��,Ҳ�����Ƕ���.�κ����������ȥ˯�ߵȴ�*/

        rc = SignalObjectAndWait(rwlck->mutex, rwlck->wr_event, ms, FALSE);
        if (rc == WAIT_OBJECT_0) {
            return 0;
        } else {
            if(rc == WAIT_TIMEOUT)
                return _RW_ETIMEOUT;
			else
				return _RW_GENERAL;
        }
    }
    rwlck->lock_type = _WR_LOCK;
    ReleaseMutex(rwlck->mutex);
    return 0;
}

/**
 *	futex_rwlock_wrunlock - ���д��
 *
 *	@rwlck:		��д��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
static INLINE_FN int futex_rwlock_wrunlock(struct futex_rwlock *rwlck)
{
#ifdef _DEBUG
	assert(rwlck->lock_type == _WR_LOCK);
#else
	if(rwlck->lock_type != _WR_LOCK)
		return _RW_GENERAL;
#endif

    WaitForSingleObject(rwlck->mutex, INFINITE);
	rwlck->wr_counter--;
    /*	��ռ�����߳�����*/
    if (rwlck->wr_counter > 0) {
        /*	����һ����ռ�����߳�*/
        SetEvent(rwlck->wr_event);
    } else if (rwlck->rd_counter > 0) {
        /*	���ѵ�ǰ���й�������߳�*/
        rwlck->lock_type = _RD_LOCK;
        PulseEvent(rwlck->rd_event);
    } else {
        /*	û�еȴ��߳�*/
        rwlck->lock_type = _NO_LOCK;
    }
    ReleaseMutex(rwlck->mutex);

	return 0;
}
#else
#error "Not yet implementation"
#endif


#ifdef __cplusplus
}
#endif

#endif	/*	__RWLOCK_HEADER__*/
#endif	/*	GCS_WITH_RWLOCK*/