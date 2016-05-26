#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

/*
*   没信号处理的system实现
*/
int
_system(const char *cmdstring)
{
    pid_t           pid;
    int             status;

    if(cmdstring == NULL)
        return 1;

    if((pid = fork()) < 0){
        status = -1;
    }else if(pid == 0){
        execl("/bin/sh",
            "sh",
            "-c",
            cmdstring, 
            (char *)0);
        _exit(127);/*execl error*/
    }else{
        while(waitpid(pid, &status, 0) < 0){
            if(errno != EINTR){
                status = -1;
                break;
            }
        }
    }

    return status;
}
