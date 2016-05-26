#include <stdio.h>
#include <pthread.h>

/*互斥结构*/
struct foo{
    int                 f_count;    /* 计数器,用以控制引用的次数 */
    pthread_mutex_t     f_lock;     /* 互斥锁变量 */
    /* 其它结构体 */
};

/* 动态申请互斥结构，并完成初始化 */
struct foo*
foo_alloc(void)
{
    struct foo*         fp;

    if((fp = malloc(sizeof(struct foo))) != NULL){
        fp->f_count = 1;    /* 初始化计数器，不必加锁，因为现在还只有一个线程在用 */
        if(pthread_mutex_init(&fp->f_lock, NULL) != 0){
            free(fp);
            return NULL;
        }
    }

    return fp;
}

/* 使用互斥结构前的动作 */
void
foo_hold(struct foo *fp)
{
    pthread_mutex_lock(&fp->f_lock); 
    ++fp->f_count;
    pthread_mutex_unlock(&fp->f_lock);      
}

/* 使用互斥结构后的动作 */
void
foo_rele(struct foo *fp)
{
    pthread_mutex_lock(&fp->f_lock);
    if(--fp->f_count == 0){ /* 最后次引用 */
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    }else{
        pthread_mutex_unlock(&fp->f_lock);
    }
}
