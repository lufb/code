#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/********************************************************/
/*  fork两次来避免僵死进程                              */
/*  第二次fork时，将第一个子进程首先退出                */
/*  让init进程来回收第二个子进程，这样避免僵尸进程      */
/********************************************************/
int main(void)
{
    pid_t           pid;
    
    if((pid = fork()) < 0){
        perror("fork");
        return -1;
    }else if(pid == 0){    /* first child */
        if((pid = fork()) < 0){
            perror("fork");
            return -2;
        }else if(pid > 0){ /*这里让父进程退出，让子进程的父进程为init来避免僵尸进程 */
            exit(0);
        }

        /*下面就是真正的第二个fork出来的子进程，是不会成为僵尸进程的*/
        /* 当然也要让其父进程先退出，这里用sleep简单实现 */
        sleep(2);
        printf("second child, parent pid = %d\n", getppid());
        exit(0);
    }

    /* 在父进程中等待第一个子进程退出 */
    /* 注意这里还是用pid */
    /*因为子进程会复制父进程空间，pid变量各用各的 */
    sleep(1);
    if(waitpid(pid, NULL, 0) != pid){
        perror("waitpid");
        return -2;
    }

    return 0;
}