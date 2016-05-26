#include <setjmp.h>
#include <stdio.h>
#include <signal.h>

static jmp_buf      env_alrm;

static void
    sig_alrm(int signo)
    {
    longjmp(env_alrm, 1);
}
int
    main()
    {
    int     n;
    char    line[1024];

    if(signal(SIGALRM, sig_alrm) == SIG_ERR){
        perror("signal");
        exit(1);
    }
    if(setjmp(env_alrm) != 0){
        perror("read timeout");
        exit(1);
    }

    alarm(10);
    if((n = read(0, line, 1024)) < 0)
        perror("read");
    alarm(0);

    write(1, line, n);

    exit(0);
}
