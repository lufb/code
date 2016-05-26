#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

pthread_t           ntid;   /*保存tid*/

void 
printids(const char *s)
{
    pid_t           pid;
    pthread_t       tid;

    pid = getpid();         /*得到进程ID*/
    /*
    得到线程ID,不用全局的ntid的原因是有可能这个值不正确
    (在create_thread返回前，线程就退出，那么这个数据就是脏数据)
    */
    tid = pthread_self();   

    printf("%s pid %u tid %u (0x%x)\n",
        s, (unsigned int)pid, 
        (unsigned int)tid,
        (unsigned int)tid);
    
}

void *
thr_fn(void *arg)
{
    printids("new thread");

    return ((void *)0);
}

int
main(void)
{
    int                 err;

    err = pthread_create(&ntid, NULL, thr_fn, NULL);
    if(err != 0){
        /*不置errno, 返回值标识是什么错，用返回值+strerror来获取相应错误 */
        printf("can't create thread:%s\n", strerror(err));
        return -1;
    }

    printids("main thread");
    sleep(1);   /*睡一秒防止主线程在子线程执行完就退出*/
    
    return 0;
}
