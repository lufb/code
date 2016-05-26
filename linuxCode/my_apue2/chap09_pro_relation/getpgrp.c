#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int
main(void)
{
    pid_t               pid;

    setbuf(stdout, NULL);
    if((pid = fork()) < 0)
    {
        perror("fork");
        exit(1);
    }
    else if(pid == 0)
    {
        sleep(2);
        printf("child pgrp[%d]\n", getpgrp());
        printf("before setsid,sessionID[%d]\n", getsid(0));
        if(setsid() == -1)
        {
            perror("setsid");
            exit(1);
        }
        printf("after setsid, sessionID[%d]\n", getsid(0));
        exit(0);
    }
    printf("parent pgrp[%d]\n", getpgrp());

    exit(0);
}

