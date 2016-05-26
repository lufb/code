#include <pthread.h>

void* thr_fn1(void*);
void* thr_fn2(void*);

/*
获取线程的退出状态
如果线程调用exit, _Exit, _exit,则整个进程都会退出
如果想只退出某个线程，进程不退出:
1 线程被取消
2 线程return
3 线程调用pthread_exit()
*/
int
main(void)
{
    int             err;
    pthread_t       tid1, tid2;
    void            *tret;

    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if(err != 0){
        printf("can't create thread: %s\n", strerror(err));
        return -1;
    }
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if(err != 0){
        printf("can't create thread: %s\n", strerror(err));
        return -1;
    }

    /*分别等待两个线程退出，并获取退出码*/
    err = pthread_join(tid1, &tret);
    if(err != 0){
        printf("can't join with thread1: %s\n", strerror(err));
        return -2;
    }
    printf("thread1 exit code: %d\n", (int)tret);

    err = pthread_join(tid2, &tret);
    if(err != 0){
        printf("can't join with thread2: %s\n", strerror(err));
        return -2;
    }
    printf("thread2 exit code: %d\n", (int)tret);

    return 0;
}

void*
thr_fn1(void *arg)
{
    printf("thread 1 returning\n");
    return ((void *)1);/*线程返回*/
}

void*
thr_fn2(void *arg)
{
    printf("thread2 exiting\n");
    pthread_exit((void *)2);/*线程退出，*/
}
