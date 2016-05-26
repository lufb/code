/*
��ϰʹ����������
�����ҳ http://www.wuzesheng.com/?p=1668
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
        /*��while��ԭ��:�п��ܻ�����ʵ�̻߳�ȡ����������������������ȡʧ��*/
        while(workq == NULL)
            pthread_cond_wait(&qready, &qlock);/*��֮����һ������������Ķ���(�Ǹ�ԭ�Ӳ���)���������ܱ�֤�����code 4�żӵ����� */
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
    pthread_cond_signal(&qready); /*������Ϣ*/
}
