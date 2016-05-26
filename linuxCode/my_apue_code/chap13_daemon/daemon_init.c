/*
守侯进程的初始化单元
守候进程初始化一般遵循以下步骤
1 调用umask将文件模式创建屏蔽字设为0
2 调用fork，然后使父进程退出
3 调用setsid创建新会话
4 将当前工作目录改为根目录
5 关闭不需要的文件描述符
6 可以打开/dev/null。这样使文件描述符，0，1，2这样就不会产生任何效果
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

void
daemo_init(const char *cmd)
{
    int                     i, fd0, fd1, fd2;
    pid_t                   pid;
    struct rlimit           rl;
    struct sigaction        sa;

    //setp 1
    unmask(0);

    if(getrlimit(RLIMIT_NOFILE, &rl) < 0){
        perror("getrlimit");

        return;
    }

    //setp 2
    if((pid = fork()) < 0){
        perror("fork");
        return;
    }else if(pid != 0){
        exit(0);
    }

    //step 3
    setsid();

    //创建第二个子进程，让第二个子进程成为守候进程
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGHUP, &sa, NULL) < 0)
        return;
    if((pid = fork()) < 0)
        return;
    else if(pid != 0)
        exit(0);

    //step 4
    if(chdir("/") < 0)
        return;

    //step 5
    if(rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for(i = 0; i < rl.rlim_max; ++i)
        close(i);

    //step 6
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    //init log
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2){
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
}
