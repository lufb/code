#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/*线程函数，模拟对锁资源加两次锁，造成死锁*/
void*
thread_func(void *arg)
{
    pthread_mutex_t         locl_lock;
    int                     err;

    if((err = pthread_mutex_lock(&locl_lock)) != 0){
        printf("lock error: %s\n", strerror(err));
    }
    /*下面将会导致死锁:在一个线程中对同一互斥量加锁两次*/
    pthread_mutex_lock(&locl_lock);
    if((err = pthread_mutex_lock(&locl_lock)) != 0){
        printf("lock error: %s\n", strerror(err));
    }
    

    pthread_mutex_unlock(&locl_lock);
    pthread_mutex_unlock(&locl_lock);

    pthread_mutex_destroy(&locl_lock);

    return ((void*)0);
}

int
main(void)
{
    pthread_t               tid;
    int                     err;
    void                    *ret;

    if((err = pthread_create(&tid, NULL, thread_func, NULL)) != 0){
        printf("can't create thread for: %s\n", strerror(err));

        return -1;
    }

    if((err = pthread_join(tid, &ret)) != 0){
        printf("can't join thread for: %s\n", strerror(err));

        return -2;
    }
    printf("thead exited with: %d\n", (int)ret);

    return 0;
}
