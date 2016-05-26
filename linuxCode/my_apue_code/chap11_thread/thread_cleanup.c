#include <pthread.h>
#include <stdio.h>


void
cleanup(void *arg)
{
    printf("cleanup: %s\n", (char *)arg);
}

void*
thr_fn1(void *arg)
{
    printf("thread1 starting\n");
    pthread_cleanup_push(cleanup, "thread 1 first handler");
    pthread_cleanup_push(cleanup, "thread 1 second handler");
    printf("thread1 push complete\n");

    if(arg)
        return ((void*)1);/*返回了就不会调清理函数了*/
    
    pthread_cleanup_pop(1);/*是非0才会调清理函数*/
    pthread_cleanup_pop(1);

    return ((void*)2);
}

void* 
thr_fn2(void *arg)
{
    printf("thread2 starting\n");
    pthread_cleanup_push(cleanup, "thread 2 first handler");
    pthread_cleanup_push(cleanup, "thread 2 second handler");
    printf("thread2 push complete\n");

    if(arg)
        return ((void*)3);
    
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

    pthread_exit((void*)4);
}

int
main(void)
{
    int             err;
    pthread_t       tid1, tid2;
    void            *tret;

    err = pthread_create(&tid1, NULL, thr_fn1, (void *)1);
    if(err != 0){
        printf("can't create thread1: %s\n", strerror(err));
        return -1;
    }
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if(err != 0){
        printf("can't create thread2: %s\n", strerror(err));
        return -2;
    }

    err = pthread_join(tid1, (void *)&tret);
    if(err != 0){
        printf("can't join thead1: %s\n", strerror(err));
        return -3;
    }
    printf("thread1 exit code: %d\n", (int)tret);

    err = pthread_join(tid2, (void *)&tret);
    if(err != 0){
        printf("can't join thead2: %s\n", strerror(err));
        return -3;
    }
    printf("thread2 exit code: %d\n", (int)tret);

    return 0;
}
