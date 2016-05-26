#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

pthread_t           ntid;   /*����tid*/

void 
printids(const char *s)
{
    pid_t           pid;
    pthread_t       tid;

    pid = getpid();         /*�õ�����ID*/
    /*
    �õ��߳�ID,����ȫ�ֵ�ntid��ԭ�����п������ֵ����ȷ
    (��create_thread����ǰ���߳̾��˳�����ô������ݾ���������)
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
        /*����errno, ����ֵ��ʶ��ʲô���÷���ֵ+strerror����ȡ��Ӧ���� */
        printf("can't create thread:%s\n", strerror(err));
        return -1;
    }

    printids("main thread");
    sleep(1);   /*˯һ���ֹ���߳������߳�ִ������˳�*/
    
    return 0;
}
