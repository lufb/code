#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define MAX_LINE        1024
static void sig_alarm(int);

/*
下面函数设置超时读的问题:
1 alarm与read之间可能就超时，这样，read就永远得不到中断
2 如果系统是自动重启的(即read被中断后，又重启)，也得不到超时
*/
int
main(void)
{
    int             n;
    char            line[MAX_LINE];

    if(signal(SIGALRM, sig_alarm) == SIG_ERR){
        perror("signal");
        return -1;
    }

    alarm(10);
    if((n = read(STDIN_FILENO, line, MAX_LINE)) < 0){
        perror("read");
        alarm(0);
        return -2;
    }
    alarm(0);

    write(STDOUT_FILENO, line, n);

    return 0;
}

static void
sig_alarm(int signo)
{
    
}
