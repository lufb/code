#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

void *
thr_fn1(void *arg)
{
    printf("thread 1 returning\n");

    return ((void *)1);
}

void *
thr_fn2(void *arg)
{
    printf("thread 2 returning\n");

    return ((void *)2);
}

int
main(void)
{
    int             err;
    pthread_t       tid1, tid2;
    void            *tret;

    if((err = pthread_create(&tid1, NULL, thr_fn1, NULL)) != 0)
    {
        printf("err1: %s\n", strerror(err));
        exit(1);
    }
    if((err = pthread_create(&tid2, NULL, thr_fn2, NULL)) != 0)
    {
        printf("err2: %s\n", strerror(err));
        exit(1);
    }

    if((err = pthread_join(tid1, &tret)) != 0)
    {
        printf("pthread_join1 error: %s\n", strerror(errno));
        exit(1);
    }
    printf("thread 1 exit code %d\n", (int)tret);

    if((err = pthread_join(tid2, &tret)) != 0)
    {
        printf("pthread_join1 error: %s\n", strerror(errno));
        exit(1);
    }
    printf("thread 2 exit code %d\n", (int)tret);

    return 0;
}

