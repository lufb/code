#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define MAX_LINE        1024
static void sig_alarm(int);

/*
���溯�����ó�ʱ��������:
1 alarm��read֮����ܾͳ�ʱ��������read����Զ�ò����ж�
2 ���ϵͳ���Զ�������(��read���жϺ�������)��Ҳ�ò�����ʱ
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
