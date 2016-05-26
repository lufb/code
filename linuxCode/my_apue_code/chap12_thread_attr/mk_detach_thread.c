/*
���������߳�
�����߳����߳��˳����ɲ���ϵͳ��������Դ
���������̷߳���
1 ����pthread_detach����
2 �ڴ����߳�ʱ�������̵߳�detachstate����(PTHREAD_CREATE_DETACHED)
*/

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

int
mk_detach_thread(void*(*fn)(void*), void *arg)
{
    int             err;
    pthread_t       tid;
    pthread_attr_t  attr;

    err = pthread_attr_init(&attr);
    if(err != 0)
        return err;

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(err == 0)
        err = pthred_create(&tid, &attr, fn, arg);
    pthred_attr_destroy(&attr);

    return (err);
}
