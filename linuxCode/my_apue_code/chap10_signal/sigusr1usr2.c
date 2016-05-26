#include <signal.h>
#include <unistd.h>
#include <stdio.h>

static void sig_usr(int);

int main(void)
{
    if(signal(SIGUSR1, sig_usr) == SIG_ERR){
        perror("signal");
        return -1;
    }

    if(signal(SIGUSR2, sig_usr) == SIG_ERR){
        perror("signal");
        return -2;
    }

    printf("install signal end\n");
    for( ; ; ){
        pause();
    }
}

static void sig_usr(int signo)
{
    sleep(10);
    if(signo == SIGUSR1){
        printf("received SIGUSR1\n");
    }else if(signo == SIGUSR2){
        printf("received SIGUSR2\n");
    }else{
        printf("received signal %d\n", signo);
    }
}
