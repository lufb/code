#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t     p_lock;

void *
thread_func(void *arg)
{
    printf("thread begin to lock first\n");
    pthread_mutex_lock(&p_lock);
    printf("thread lock first sucess\n");
#if 0
    printf("thread begin to lock second\n");
    pthread_mutex_lock(&p_lock);        /*  ÷ÿ∏¥º”À¯£¨ª·À¿À¯    */
    printf("thread lock second sucess\n");
#endif

    return ((void *)0);
}

int
main(void)
{
    pthread_t           tid;
    int                 err;
    void                *ret;

    pthread_mutex_init(&p_lock, NULL);
    if((err = pthread_create(&tid, NULL, thread_func, NULL)) != 0)
    {
        fprintf(stderr, "create thread error: %s\n", strerror(err));
        pthread_mutex_destroy(&p_lock);
        exit(1);
    }

    if(pthread_join(tid, &ret) != 0)
    {
        perror("pthread_join");
        pthread_mutex_destroy(&p_lock);
        exit(1);
    }
    pthread_mutex_destroy(&p_lock);

    printf("thread return with code: %d\n", ret);

    exit(0);
}

