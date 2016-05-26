#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>

char            *env_init[] = {"USER=unknown",
                               "PATH=/tmp",
                               NULL
                              };

int
main(void)
{
    pid_t               pid;

    if((pid = fork()) < 0){
        perror("fork1");
        exit(1);
    }else if(pid == 0){
        if(execle("./echoall", 
            "echoall", 
            "myarg1", 
            "MY_ARG2",
            (char *)0,
            env_init) < 0){
            perror("execle");
            exit(1);
        }
    }

    if(waitpid(pid, NULL, 0) < 0){
        perror("waitpid");
        exit(1);
    }

    if((pid = fork()) < 0){
        perror("fork2");
        exit(1);
    }else if(pid == 0){
        if(execlp("echoall",
            "echoall",
            "only 1 arg",
            (char *)0) < 0){
            perror("execlp");
            exit(1);
        }
    }

    exit(0);
}
