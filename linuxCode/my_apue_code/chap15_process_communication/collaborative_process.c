/*
    使用协同进程:一个程序产生某个过滤程序的输入，同时又读该过滤的输出。

    原理:
     父进程                               子进程(协同进程)
   |--------|            管道1            |---------|         
   | pfd1[1]| --------------------------> | stdin   |
   |        |            管道2            |         |
   | pfd2[0]| <-------------------------- | stdout  |
   |________|                             |_________|   
    
*/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define     ADD2            "./filter_sum"  /*由filter_sum.c编译出来的求和程序*/

static void
sig_pipe(int signo)
{
    printf("SIGPIPE caught\n");
    exit(1);
}

int
main(void)
{
    int             n, fd1[2], fd2[2];
    pid_t           pid;
    char            line[1024];

    if(signal(SIGPIPE, sig_pipe) < 0){
        perror("signal");
        return -1;
    }

    if(pipe(fd1) < 0 || pipe(fd2) < 0){
        perror("pipe");
        return -2;
    }

    if((pid = fork()) < 0){
        perror("fork");
        return -3;
    }else if(pid > 0){
        close(fd1[0]);
        close(fd2[1]);
        while(fgets(line, sizeof(line), stdin) != NULL){
            n = strlen(line);
            if(write(fd1[1], line, n) != n){
                perror("write");
                return -4;
            }
            if((n = read(fd2[0], line, sizeof(line))) < 0){
                perror("read from pipe");
                return -5;
            }
            if(n == 0){
                printf("child closed pipe");
                return -6;
            }
            line[n] = 0;
            if(fputs(line, stdout) == EOF){
                perror("fputs");
                return -7;
            }
        }
        if(ferror(stdin)){
            perror("stdin");
            return -8;
        }
        return 0;
    }else{
        close(fd1[1]);
        close(fd2[0]);
        if(fd1[0] != STDIN_FILENO){
            dup2(fd1[0], STDIN_FILENO);
            close(fd1[0]);
        }
        if(fd2[1] != STDOUT_FILENO){
            dup2(fd2[1], STDOUT_FILENO);
            close(fd2[1]);
        }

        if(execl(ADD2, "add2", (char *)0) < 0){/*执行协同程序*/
            perror("execl");
            return -9;
        }

        exit(0);
    }
}
