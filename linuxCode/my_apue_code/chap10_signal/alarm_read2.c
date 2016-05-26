#include <stdio.h>
#include <longjmp.h>
#include <unistd.h>

#define MAX_LINE    1024

static jmp_buf      env_alrm;

static void sig_alarm(int);

/*
该函数能解决alarm_read.c中的两个问题
*/
int
main(void)
{
    int         n;
    char        line[MAX_LINE];

    if(signal(SIGALRM, sig_alarm) == SIG_ERR){
        perror("signal");
        return -1;
    }
    if(setjmp(env_alrm) != 0){
        perror("setjmp");
        return -2;
    }

    alarm(10);
    if((n = read(STDIN_FILENO, line, MAX_LINE)) < 0){
        perror("read");
        return -3;
    }
    alarm(0);
}

static void sig_alarm(int signo)
{
    longjmp(env_alrm, 1);
}