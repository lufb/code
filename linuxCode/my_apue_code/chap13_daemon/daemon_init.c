/*
�غ���̵ĳ�ʼ����Ԫ
�غ���̳�ʼ��һ����ѭ���²���
1 ����umask���ļ�ģʽ������������Ϊ0
2 ����fork��Ȼ��ʹ�������˳�
3 ����setsid�����»Ự
4 ����ǰ����Ŀ¼��Ϊ��Ŀ¼
5 �رղ���Ҫ���ļ�������
6 ���Դ�/dev/null������ʹ�ļ���������0��1��2�����Ͳ�������κ�Ч��
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

    //�����ڶ����ӽ��̣��õڶ����ӽ��̳�Ϊ�غ����
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
