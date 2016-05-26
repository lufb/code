#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "./pr_exit.h"

void
pr_other(struct rusage *usage)
{
    if(usage == NULL)
        return;
    printf("usertime[%d]systime[%d]pagefaults[%d]signalreceived[%d]\n\n",
           usage->ru_utime.tv_sec,
           usage->ru_stime.tv_sec,
           usage->ru_majflt,
           usage->ru_nsignals);
}

int
main(void)
{
    pid_t           pid;
    int             status;
    struct rusage   usage;

    if((pid = fork()) < 0)
    {
        perror("fork");
        exit(1);
    }
    else if(pid == 0)
    {
        exit(7);
    }

    if(wait3(&status, 0, &usage) != pid)
    {
        perror("wait");
        exit(1);
    }
    pr_exit(status);
    pr_other(&usage);

    if((pid = fork()) < 0)
    {
        perror("fork2");
        exit(1);
    }
    else if(pid == 0)
    {
        abort();
    }

    if(wait3(&status, 0, &usage) != pid)
    {
        perror("wait2");
        exit(1);
    }
    pr_exit(status);
    pr_other(&usage);

    if((pid = fork()) < 0)
    {
        perror("fork3");
        exit(1);
    }
    else if(pid == 0)
    {
        status /= 0;
    }

    if(wait3(&status, 0, &usage) != pid)
    {
        perror("wait3");
        exit(1);
    }
    pr_exit(status);
    pr_other(&usage);

    exit(0);
}

