#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

void
cleanup(void *arg)
{
    printf("cleanup: %s\n", (char *)arg);
}

void *
thread_fn1(void *arg)
{
    printf("thread 1 start\n");
    pthread_cleanup_push(cleanup, "thread 1 first handler");
    pthread_cleanup_push(cleanup, "thread 1 second handler");
    printf("thread 1 push complete\n");
    if(arg)
        return ((void *)1);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return ((void *)1);

}

void *
thread_fn2(void *arg)
{
    printf("thread 2 start\n");
    pthread_cleanup_push(cleanup, "thread 2 first handler");
    pthread_cleanup_push(cleanup, "thread 2 second handler");
    printf("thread 2 complete\n");
    if(arg)
        pthread_exit ((void *)2);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return ((void *)2);
}

int
main(void)
{
    int             err;
    pthread_t       tid1, tid2;
    void            *tret;

    if((err = pthread_create(&tid1, NULL, thread_fn1, (void *)1)) != 0)
    {
        printf("error1: %s\n", strerror(err));
        exit(1);
    }
    if((err = pthread_create(&tid2, NULL, thread_fn2, (void *)1)) != 0)
    {
        printf("error2: %s\n", strerror(err));
        exit(1);
    }

    if((err = pthread_join(tid1, &tret)) != 0)
    {
        printf("pthread_join 1 error: %s\n", strerror(errno));
        exit(1);
    }
    printf("thread 1 exit code: %d\n", tret);
    if((err = pthread_join(tid2, &tret)) != 0)
    {
        printf("pthread_join 2 error: %s\n", strerror(errno));
        exit(1);
    }
    printf("thread 2 exit code: %d\n", tret);

    exit(0);
}
