#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

int system(const char *cmdstring)
{
    pid_t       pid;
    int         status;

    if(NULL == cmdstring){
        return 1;
    }

    if((pid = fork()) < 0){
        status = -1;
    }else if(pid == 0){
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
    }else{
        /*it's error with below code*/
        /*
        pid_t           lastpid;
        while((lastpid = wait(&status)) != pid && lastpid != -1)
            ;
        */
        /*
        因为:如果调用者在这调用system之前已经创建了子进程
        这里就有可能返回的是调用者创建的子进程的返回,
        就导致这个pid被while语句丢弃，
        调用者就获取不到这个pid了
        */
        while(waitpid(pid, &status, 0) < 0){
            if(errno != EINTR){
                status = -1;
                break;
            }
        }
    }

    return status;
}

void pr_exit(int status)
{
    printf("%d\n", status);
    if(WIFEXITED(status))
        printf("normal return, exit status[%d]\n", WEXITSTATUS(status));
    else if(WIFSIGNALED(status))
        printf("abnormal termination, signal num [%d]\n", WTERMSIG(status));
    else if(WIFSTOPPED(status))
        printf("abnormal termination, signal num [%d]\n", WSTOPSIG(status));
}

int main(void)
{
    int         status;

    if((status = system("date")) < 0){
        printf("system() error\n");
        return -1;
    }
    pr_exit(status);

    if((status = system("nosuchcmd")) < 0){
        printf("system() error\n");
        return -1;
    }
    pr_exit(status);

    if((status = system("who; exit 44")) < 0){
        printf("system() error\n");
        return -1;
    }
    pr_exit(status);

    return 0;
}
