#ifdef GCS_WITH_RWLOCK
/*
 *	lib/win32/rwlock.h
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

#ifndef		__RWLOCK_HEADER__
#define		__RWLOCK_HEADER__

#include	"features.h"

/**
 *	读写锁
 */
#define	_RW_ETIMEOUT	-1
#define	_RW_GENERAL		-2

#define	WAIT_FOREVER	0xFFFFFFFF
	
#ifdef WIN32
#define	_NO_LOCK		0
#define	_RD_LOCK		1
#define	_WR_LOCK		2
struct futex_rwlock {
	HANDLE			mutex;				/*	互斥句柄*/
	HANDLE			rd_event;			/*	读等待句柄*/
	HANDLE			wr_event;			/*	写等待句柄*/

	volatile int	rd_counter;			/*	读计数*/
    volatile int	wr_counter;			/*	写计数*/
    volatile int	lock_type;			/*	锁类型*/
};
#else
#error "Not yet implementation"
#endif

#ifdef	WIN32
/**
 *	不要定义太多的读写锁,每个读写锁会占用3个句柄
 *	过多的句柄会严重导致系统的性能
 *	每个句柄属于一个内核对象,其实它的效率相比
 *	WIN提供的轻量级的CriticalSection临界区来说
 *	每个内核对象的操作都会导致操作系统进入一个
 *	用户态到内核态的转换过程,(压入所有的CPU寄存器,IO端口等, 术语切换TSS段)
 *	导致发生一个软中断, Inter 习惯叫 程序进入陷阱trap
 *	这个过程是相当消耗时间的
 *	在I386/686系统编程中,程序分为了内核态和用户态
 *	操作系统运行的代码都属于内核态.我们的程序运行在用户态
 *	WIN提供的轻量级的CriticalSection临界区
 *	属于利用I386/686 机器的CAS Compare And Swap的方式来实现的
 *	不存在状态切换,所以速度相当快
 *	要是m_hMutex能使用这个WIN提供的轻量级的CriticalSection临界区就完美了
 *	目前我在VC60的环境下,只能这样了.在VS2008中已经有一个相对完美的解决方案
 *
 *	有时候其实我真的很喜欢LINUX,它这个锁上面,提供了一个用户态的自旋锁.到目前为止
 *	MS还没有在VC60中提供用户态的spinlock补丁包
 *
 *								--	阳凌
 */

/**
 *	请特别注意:SignalObjectAndWait函数和LINUX的pthread_cond_wait函数是有点差异的
 *	他们的共同点:在睡眠之前都会原子的操作h_Mutex的解锁,然后等待.
 *	LINUX的pthread_cond_wait 函数 再重新苏醒过来后
 *	会重新加上m_hMutex这个临界锁并且苏醒与加锁的过程是原子的
 *	在WIN下我们很做到这点
 *	是WIN下的SignalObjectAndWait的函数再苏醒后,是不会再加m_hMutex的临界锁的
 *	所以在WIN下面,要求在苏醒后是不能再处理内部的保护变量的
 *	m_nRDNum, m_nWRNum, m_LockType 3个变量的
 *	所以程序的编写逻辑上面和LINUX是有区别的
 *	请在移植LINUX下的时候,注意这点.在移植LINUX的时候
 *	可以考虑2个移植方案
 *	1:直接使用 库函数.pthread_rwlock_rdlock 等函数
 *	2:参考 UNIX/LINUX 的圣书<APUE2 高级UNIX环境编程(第二版)>里面介绍的读写锁的具体实现方式,
 *	以及我们在函数使用的时候有什么地方需要注意的地方
 *	这书详细描述了库函数是如何实现的,以及如何利用mutex和线程同步函数pthread_signal等组合实现读写锁的
 *
 *									--	阳凌
 */

/**
 *	futex_rwlock_init - 初始化读写锁
 *
 *	@rwlck:		读写锁
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static INLINE_FN int futex_rwlock_init(struct futex_rwlock *rwlck)
{
	memset(rwlck, 0, sizeof(struct futex_rwlock));

	/* 创建用于保护内部数据的互斥量*/
	rwlck->mutex = CreateMutex(NULL, FALSE, NULL);
    /* 创建用于同步共享访问线程的事件（手动事件）*/
    rwlck->rd_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    /* 创建用于同步独占访问线程的事件（自动事件）*/
    rwlck->wr_event = CreateEvent(NULL, FALSE, FALSE, NULL);

	return 0;
}

/**
 *	futex_rwlock_destroy - 销毁读写锁
 *
 *	@rwlck:		读写锁
 *
 *	return
 *		0		成功
 *		!0		失败
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
 *	futex_rwlock_rdlock - 获取读锁
 *
 *	@rwlck:		读写锁
 *	@ms:		超时时间(单位:毫秒)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static INLINE_FN int futex_rwlock_rdlock(
			struct futex_rwlock *rwlck, unsigned int ms)
{
	unsigned int	rc;

    WaitForSingleObject(rwlck->mutex, INFINITE);
	rwlck->rd_counter++;
    if (rwlck->lock_type == _WR_LOCK) {
		/*	当前是写锁,我去睡眠等待*/
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
 *	futex_rwlock_rdunlock - 解除读锁
 *
 *	@rwlck:		读写锁
 *
 *	return
 *		0		成功
 *		!0		失败
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
            /*	唤醒一个独占访问线程*/
            rwlck->lock_type = _WR_LOCK;
            SetEvent(rwlck->wr_event);
        } else {
            /*	没有等待线程*/
            rwlck->lock_type = _NO_LOCK;
        }
    }
    ReleaseMutex(rwlck->mutex);

	return 0;
}

/**
 *	futex_rwlock_wrlock - 获取写锁
 *
 *	@rwlck:		读写锁
 *	@ms:		超时时间(单位:毫秒)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
static INLINE_FN int futex_rwlock_wrlock(
		struct futex_rwlock *rwlck, unsigned int ms)
{
	unsigned int rc;

    WaitForSingleObject(rwlck->mutex, INFINITE);
	rwlck->wr_counter++;
    if (rwlck->lock_type != _NO_LOCK) {
		/*	当前已经有人持有锁,可能是写锁,也可能是读锁.任何情况都必须去睡眠等待*/

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
 *	futex_rwlock_wrunlock - 解除写锁
 *
 *	@rwlck:		读写锁
 *
 *	return
 *		0		成功
 *		!0		失败
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
    /*	独占访问线程优先*/
    if (rwlck->wr_counter > 0) {
        /*	唤醒一个独占访问线程*/
        SetEvent(rwlck->wr_event);
    } else if (rwlck->rd_counter > 0) {
        /*	唤醒当前所有共享访问线程*/
        rwlck->lock_type = _RD_LOCK;
        PulseEvent(rwlck->rd_event);
    } else {
        /*	没有等待线程*/
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