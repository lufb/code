#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
    pid_t       pid;
    int         status;

    //normal termination
    if((pid = fork()) < 0){
        perror("fork");
        return -1;
    }else if(pid == 0){
        exit(7);
    }
    
    if(wait(&status) != pid){
        printf("wait error\n");
        return -2;
    }
    pr_exit(status);

    //abnormal termination with 6
    if((pid = fork()) < 0){
        perror("fork");
        return -3;
    }else if(pid == 0){
        abort();            /* <signal.h> defined with 6 */
    }
    
    if(wait(&status) != pid){
        printf("wait error\n");
        return -4;
    }
    pr_exit(status);
    
    //abnormal termination with 8
    if((pid = fork()) < 0){
        perror("fork");
        return -5;
    }else if(pid == 0){
        status = status/0;  /* 异常 */
    }
 
    if(wait(&status) != pid){
        printf("wait error\n");
        return -6;
    }
    pr_exit(status);
    
    if((pid = fork()) < 0){
        perror("fork");
        return -1;
    }else if(pid == 0){//子进程，会是僵尸进程
        printf("pid [%d]\n", getpid());
        sleep(2);
    }

    while(1) sleep(1);

    return 0;
}
