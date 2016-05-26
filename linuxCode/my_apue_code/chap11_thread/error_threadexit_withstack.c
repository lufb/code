#include <pthread.h>

struct foo{
    int     a, b, c, d;
};

void
printfoo(const char *s, const struct foo *fp)
{
    printf(s);
    printf("structure at 0x%x\n", (unsigned int)fp);
    printf("    foo.a = %d\n", fp->a);
    printf("    foo.b = %d\n", fp->b);
    printf("    foo.c = %d\n", fp->c);
    printf("    foo.d = %d\n", fp->d);
}

void*
thr_fn1(void *arg)
{
    struct foo foo = {1, 2, 3, 4};
    printfoo("thread1:\n", &foo);
    /*
    出错了，线程退出时，数据在栈上，得到的数据可能是脏数据
    解决办法:可用malloc在堆上返回解决。
    */
    pthread_exit((void *)&foo);
}

void*
thr_fn2(void *arg)
{
    printf("thread2 : ID is%u\n", pthread_self());
    pthread_exit((void*)0);
}

int
main(void)
{
    int             err;
    pthread_t       tid1, tid2;
    struct foo      *fp;

    err = pthread_create(&tid1, NULL, thr_fn1, NULL);
    if(err != 0){
        printf("can't create thread: %s\n", strerror(err));
        return -1;
    }
    err = pthread_join(tid1, (void*)&fp);
    if(err != 0){
        printf("can't join thread1: %s\n", strerror(err));
        return -2;
    }

    sleep(1);
    printf("parent starting second thread\n");
    err = pthread_create(&tid2, NULL, thr_fn2, NULL);
    if(err != 0){
        printf("can't create thread2: %s\n", strerror(err));
        return -1;
    }
    sleep(1);
    printfoo("parent: \n", fp);
    
    return 0;
}
