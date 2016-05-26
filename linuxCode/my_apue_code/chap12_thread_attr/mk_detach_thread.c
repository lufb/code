/*
创建分离线程
分离线程在线程退出后，由操作系统来回收资源
创建分享线程方法
1 调用pthread_detach函数
2 在创建线程时，设置线程的detachstate属性(PTHREAD_CREATE_DETACHED)
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
