/*
练习使用条件变量
配合网页 http://www.wuzesheng.com/?p=1668
->hread 1 lock->thread 1 wait-> thread 1 unlock(in wait)
->thread 2 lock->thread 2 signal->thread 2 unlock
->thread 1 lock(in wait)->thread 1 unlock
*/

#include <pthread.h>

struct msg{
    struct msg      *m_next;
    /*other info*/
};

struct msg *workq;
pthread_cond_t  qready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;


/*thread 1*/
void
process_msg(void)
{
    struct msg          *mp;

    for( ; ; ){
        pthread_mutex_lock(&qlock);
        /*用while的原因:有可能还有其实线程获取到了这个条件，导致这儿获取失败*/
        while(workq == NULL)
            pthread_cond_wait(&qready, &qlock);/*这之间有一个解锁与加锁的动作(是个原子操作)，这样才能保证下面的code 4才加得上锁 */
        mp = workq;
        workq = mp->m_next;
        pthread_mutex_unlock(&qlock);
        /*process the message mp*/
    }
}

/*thread 2*/
void
enqueue_msg(struct msg *mp)
{
    pthread_mutex_lock(&qlock);/// code 4
    mp->m_next = workq;
    workq = mp;
    pthread_mutex_unlock(&qlock);
    pthread_cond_signal(&qready); /*发送信息*/
}
