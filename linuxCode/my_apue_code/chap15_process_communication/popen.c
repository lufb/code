/*
   使得popen重写pipe_advance.c
   popen其实完全可用pipe实现，只不过，它把操作简化了,能少写代码:
    它创建管道=>调用fork创建进程=>关闭不使用的一端=>执行shell命令，然后等待终止
*/

#include <sys/wait.h>
#include <stdio.h>

/*
shell命令${PAGER:-more}意思
如果PAGER存在，且值非空，则使用PAGER值，否则使用more命令
*/
#define PAGER                   "${PAGER:-more}"

int 
main(int argc, char *argv[])
{
    char                line[1024];
    FILE                *fpin, *fpout;

    if(argc != 2){
        printf("usage: a.out <pathname>");
        return -1;
    }

    if((fpin = fopen(argv[1], "r")) == NULL){   /*读,注意这里是打开文件*/
        perror("fopen fpin");
        return -2;
    }
    if((fpout = popen(PAGER, "w")) == NULL){    /*w连接到PAGER的标准输入，这之间其实执行了fork,dup, wait操作*/
        perror("popen fpout");
        return -3;
    }

    while(fgets(line, sizeof(line), fpin) != NULL){
        if(fputs(line, fpout) == EOF){
            perror("fputs");
            return -4;
        }
    }
    if(ferror(fpin)){
        perror("fgets");
        return -5;
    }
    if(pclose(fpout) == -1){
        perror("fclose");
        return -6;
    }

    return 0;
}
