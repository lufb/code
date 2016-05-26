#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

pthread_mutex_t         lock;

/*加了锁后就睡25秒，好让第二个线程加不上锁*/
void*
thread_fcn1(void *arg)
{
   pthread_mutex_lock(&lock);
   sleep(5);
   printf("thred 1 is going to unlock\n");
   pthread_mutex_unlock(&lock);

   return ((void*)0);
}

void*
thread_fcn2(void *arg)
{
    int                 err;

    while(1){
        if((err = pthread_mutex_trylock(&lock)) != 0){
            /*会打印类似如下信息: "thread 2 trylock error: Device or resource busy" */
            printf("thread 2 trylock error: %s\n", strerror(err));
            sleep(1);
            continue;
        }
        printf("thread 2 lock sucess\n");
        break;
    }

    pthread_mutex_unlock(&lock);

    return ((void*)1);
}

void 
init(void)
{
    pthread_mutex_init(&lock, NULL);
}

void
destroy(void)
{
    pthread_mutex_destroy(&lock);
}
    

int
main(void)
{
    pthread_t               tid1, tid2;
    int                     err;
    void                    *thread_ret;

    if((err = pthread_create(&tid1, NULL, thread_fcn1, NULL)) != 0){
        printf("can't create thread: %s\n", strerror(err));

        return -1;
    }
    if((err = pthread_create(&tid2, NULL, thread_fcn2, NULL)) != 0){
        printf("can't create thread: %s\n", strerror(err));

        return -2;
    }

    if((err = pthread_join(tid1, &thread_ret)) != 0){
        printf("can't join thread1: %s\n", strerror(err));

        return -3;
    }
    printf("thead1 exit with: %d\n", (int)thread_ret);
    if((err = pthread_join(tid2, &thread_ret)) != 0){
        printf("can't join thread1: %s\n", strerror(err));

        return -4;
    }
    printf("thead2 exit with: %d\n", (int)thread_ret);

    return 0;
}
