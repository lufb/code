#include <signal.h>
#include "./pr_mask.h"

static void
sig_quit(int signo)
{
    printf("caught SIGQUIT\n");
    if(signal(SIGQUIT, SIG_DFL) == SIG_ERR)
        perror("signal2");
}

int
main(void)
{
    sigset_t        newmask, oldmask, pendmask;

    if(signal(SIGQUIT, sig_quit) == SIG_ERR){
        perror("signal");
        exit(1);
    }

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
        perror("sigprocmask");
        exit(1);
    }

    sleep(5);

    if(sigpending(&pendmask) < 0){
        perror("sigpending");
        exit(1);
    }
    if(sigismember(&pendmask, SIGQUIT))
        printf("\nSIGQUIT pending\n");

    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
        perror("sigprocmask2");
        exit(1);
    }
    printf("SIGQUIT unblocked\n");

    sleep(5);

    exit(0);
}
