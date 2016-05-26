/*
    该程序练习使用管道，父进程向子进程发数据
    管道缺点:   (1)有些系统管道是半双工的；
                (2)仅适合亲属进程间通信
    规则:       (1)当读一个写端已关闭的管道时，在所有数据都读完后，返回0
                (2)如写一个读端已关闭的管道时，产生SIGPIPE信号
                (3)在写管道时，如果多进程向同一管道写小于BUF的数据，不会错乱，但如果写超过BUF,则会相互穿插，即错乱
*/

#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int 
main(int argc, char *argv[])
{
    int                     n;
    int                     fd[2];
    pid_t                   pid;
    char                    line[1024];

    if(pipe(fd) < 0){ /*建立管道，fd[0]用于读,fd[1]用于写*/
        perror("pipe");
        return -1;
    }

    if((pid = fork()) < 0){
        perror("fork");
        return -2;
    }else if(pid > 0){
        close(fd[0]);
        write(fd[1], "hello world\n", 12);
    }else{
        close(fd[1]);
        n = read(fd[0], line, sizeof(line));
        write(STDOUT_FILENO, line, n);
    }

    return 0;
}
