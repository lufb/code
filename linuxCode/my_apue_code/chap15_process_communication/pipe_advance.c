/*
    稍高一些的pipe应用:将文件复制到分布程序
*/
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DEF_PAGER   "/bin/more"

int
main(int argc, char *argv[])
{
    int                     n;
    int                     fd[2];
    pid_t                   pid;
    char                    *pager, *argv0;
    char                    line[1024];
    FILE                    *fp;

    if(argc != 2){
        printf("usage: a.out <pathname>\n");
        return -1;
    }

    if((fp = fopen(argv[1], "r")) == NULL){
        printf("open file [%s] error: %s\n", argv[1], strerror(errno));
        return -2;
    }
    
    if(pipe(fd) < 0){
        perror("pipe");
        return -3;
    }

    if((pid = fork()) < 0){
        perror("fork");
        return -4;
    }else if(pid > 0){
        close(fd[0]);
        while(fgets(line, sizeof(line), fp) != NULL){
            n = strlen(line);
            if(write(fd[1], line, n) != n){
                perror("write");
                return -5;
            }
        }
        if(ferror(fp)){ /*读文件结束或者读文件出错，fgets都会返回NULL,用ferror来判断是不是发生了错误*/
            printf("read file [%s] error\n", argv[1]);
            return -6;
        }

        close(fd[1]);

        /*等待子进程退出*/
        if(waitpid(pid, NULL, 0) < 0){
            perror("waitpid");
            return -7;
        }

        return 0;
    }else{
        close(fd[1]);
        /*要先判断是不相等了才进行文件符复制*/
        if(fd[0] != STDIN_FILENO){
            if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO){
                perror("dup2");
                return -8;
            }
            close(fd[0]);/*要关闭该文件描述符，因为被复制出来了，共享文件描述符，以后都不用了*/
        }
        /*文件描述符已被复制，子进程的标准输入是管道了，即复制前的fd[0]*/
        /*获取页的大小*/
        if((pager = getenv("PAGER")) == NULL)
            pager = DEF_PAGER;
        if((argv0 = strrchr(pager, '/')) != NULL)
            argv0++;
        else
            argv0 = pager;

        if(execl(pager, argv0, (char *)0) < 0){
            perror("execl");
            return -9;
        }
        return 0;
    }
    
}
