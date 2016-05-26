#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void
sig_usr(int signo)
{
    if(signo == SIGUSR1)
        printf("received SIGUSR1\n");
    else if(signo = SIGUSR2)
        printf("received SIGUSR2\n");
    else
        printf("received signum:%d\n", signo);
}

int
main(void)
{
    if(signal(SIGUSR1, sig_usr) == SIG_ERR){
        perror("signal1");
        exit(1);
    }
    if(signal(SIGUSR2, sig_usr) == SIG_ERR){
        perror("signal2");
        exit(1);
    }

    for( ; ; )
        pause();
}
