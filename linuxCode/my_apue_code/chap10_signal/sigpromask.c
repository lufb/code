#include <signal.h>
#include <unistd.h>
#include <stdio.h>

static void sig_quit(int);

int
main(void)
{
    sigset_t        newmask, oldmask, pendmask;

    if(signal(SIGQUIT, sig_quit) == SIG_ERR){
        printf("signal error\n");

        return -1;
    }

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
        printf("sigprocmask error\n");

        return -2;
    }

    sleep(5);

    if(sigpending(&pendmask) < 0){
        printf("sigpending error\n");

        return -3;
    }
    if(sigismember(&pendmask, SIGQUIT)){
        printf("sigismember error\n");
    }

    /*Reset signal mask */
    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
        printf("sigprocmask error\n");

        return -5;
    }
    printf("SIGQUIT unblocked\n");

    sleep(5);

    return (0);
}

static void
sig_quit(int signo)
{
    printf("caught SIGQUIT\n");
    if(signal(SIGQUIT, SIG_DFL) == SIG_ERR){
        printf("signal error\n");
    }
}
