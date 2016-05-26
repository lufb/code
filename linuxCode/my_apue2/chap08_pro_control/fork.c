#include <sys/wait.h>
#include <stdlib.h>
#include "./pr_exit.h"

int
main(void)
{
    pid_t           pid;
    int             status;

    if((pid = fork()) < 0)
    {
        perror("fork");
        exit(1);
    }
    else if(pid == 0)
        exit(7);

    if(wait(&status) != pid)
    {
        perror("wait");
        exit(1);
    }
    pr_exit(status);

    if((pid = fork()) < 0)
    {
        perror("fork2");
        exit(1);
    }
    else if(pid == 0)
    {
        abort();
    }

    if(wait(&status) != pid)
    {
        perror("wait2");
        exit(1);
    }
    pr_exit(status);

    if((pid = fork()) < 0)
    {
        perror("fork3");
        exit(1);
    }
    else if(pid == 0)
    {
        status /= 0;
    }

    if(wait(&status) != pid)
    {
        perror("wait3");
        exit(1);
    }
    pr_exit(status);
    
    exit(0);
}
