#include <signal.h>
#include <unistd.h>
#include <stdio.h>

unsigned int        sleep2(unsigned int);
static void         sig_int(int);

int 
main(void)
{
    unsigned int       unslept;

    if(signal(SIGINT, sig_int) == SIG_ERR){
        perror("signal");
        return 0;
    }
    unslept = sleep2(5);
    printf("sleep2 returned:%d\n", unslept);

    return (0);
}

static void
sig_int(int signo)
{
    volatile int    k = 0;
    int             i,j;

    printf("\nsig_int starting\n");
    for(i = 0; i < 300000; ++i)
        for(j = 0; j < 300000; ++i)
            k += i*j;

    printf("sig_int finished\n");
}
