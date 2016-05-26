/*
守护进程的惯例:
1:  若守侯进程使用文件锁，则锁文件通常放在/var/run/*.pid下
2:  若守候进程支持配置选项，配置文件通常放在/etc/*.conf
3:  若守候进程支持自动启动，则放在在系统的脚本中加配置，如/etc/init.d/* 
4:  若守候进程支持配置选项，且配置文件有变时，好重新更新配置，我们安装SIGHUP信号
*/

/*
    下面的例子是实例，守候进程在配置有变时，重读配置
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
        case SIGHUP:     /* 收到配置变更的信号 */
            syslog(LOG_INFO, "Re-reading configuration");
            reread();
            break;
        case SIGTERM:   /* kill 的信号 */
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
    
    daemo_init(cmd); /*成为后台进程*/

    if(already_running()){  /*确保只单一实例*/
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
        守候进程的处理代码
    */

    exit(0);
}
