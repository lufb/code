/*
    此程序利用popen去调用一个filter的过滤程序，filter由filter.c编译而成
    popen经常用于调用过滤程序
*/

#include <sys/wait.h>
#include <stdio.h>

#define FILTER              "./filter"    /*调用的可挂靠的过滤程序名称,在此测试程序中是由filter.c编译生成的*/

int
main(void)
{
    char                    line[1024];
    FILE                    *fpin;

    if((fpin = popen(FILTER, "r")) == NULL){/*将fpin连接到FILTER的标准输出*/
        perror("popen");
        return -1;
    }

    for( ; ; ){
        fputs("prompt> ", stdout);
        fflush(stdout);
        if(fgets(line, sizeof(line), fpin) == NULL)/*从管道里读*/
            break;
        if(fputs(line, stdout) == EOF){
            perror("fputs");
            return -2;
        }
    }

    if(pclose(fpin) == -1){
        perror("pclose");
        return -3;
    }
    putchar('\n');
    return 0;
}
