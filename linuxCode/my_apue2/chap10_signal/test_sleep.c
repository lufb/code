#include "./sleep.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static void
sig_int(int signo)
{
    int                 i,j;
    volatile int        k;

    printf("\nsig_int starting\n");
    for(i = 0; i < 300000; ++i)
        for(j = 0; j < 4000; ++j)
            k += i*j;
    printf("sig_int finished\n");
}

int
main()
{
    unsigned int        unslept;

    if(signal(SIGINT, sig_int) == SIG_ERR){
        perror("signal");
        exit(1);
    }

    unslept = sleep2(5);
    printf("sleep2 returned :%d\n", unslept);

    exit(0);
}
