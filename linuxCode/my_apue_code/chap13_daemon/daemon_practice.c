/*
�ػ����̵Ĺ���:
1:  ���غ����ʹ���ļ����������ļ�ͨ������/var/run/*.pid��
2:  ���غ����֧������ѡ������ļ�ͨ������/etc/*.conf
3:  ���غ����֧���Զ��������������ϵͳ�Ľű��м����ã���/etc/init.d/* 
4:  ���غ����֧������ѡ��������ļ��б�ʱ�������¸������ã����ǰ�װSIGHUP�ź�
*/

/*
    �����������ʵ�����غ�����������б�ʱ���ض�����
*/
#include <pthread.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>

sigset_t    mask;
extern int  already_runnint(void);

void
reread()
{

}

void *
thr_fn(void *arg)
{
    int                     err, signo;

    for( ; ; ){
        err = sigwait(&mask, &signo);
        if(err != 0){
            syslog(LOG_ERR, "sigwait failed");
            exit(1);
        }
        switch(signo){
        case SIGHUP:     /* �յ����ñ�����ź� */
            syslog(LOG_INFO, "Re-reading configuration");
            reread();
            break;
        case SIGTERM:   /* kill ���ź� */
            syslog(LOG_INFO, "got SIGTERM; exiting");
            exit(0);
        default:
            syslog(LOG_INFO, "Unexpected signal %d\n", signo);
        }
    }

    return (void*)0;
}

int
main(int argc, char *argv[])
{
    int                     err;
    pthread_t               tid;
    char                    *cmd;
    struct sigaction        sa;

    if((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        ++cmd;
    
    daemo_init(cmd); /*��Ϊ��̨����*/

    if(already_running()){  /*ȷ��ֻ��һʵ��*/
        syslog(LOG_ERR, "daemon had running");
        exit(1);
    }
    
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGHUP, &sa, NULL) < 0){
        syslog(LOG_ERR, "sigaction error: %s", strerror(errno));
        exit(2);
    }
    sigfillset(&mask);
    if((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0){
        syslog(LOG_ERR, "pthread_sigmask error: %s", strerror(errno));
        exit(3);
    }

    err = pthread_create(&tid, NULL, thr_fn, 0);
    if(err != 0){
        syslog(LOG_ERR, "pthread_create error: %s", strerror(errno));
        exit(4);
    }

    /*
        �غ���̵Ĵ������
    */

    exit(0);
}
