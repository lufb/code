/*
    此程序是对两个数求和的过滤程序
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int
main(void)
{
    int                 n, int1, int2;
    char                line[1024];

    while((n = read(STDIN_FILENO, line, sizeof(line))) > 0){
        line[n] = 0;
        if(sscanf(line, "%d%d", &int1, &int2) == 2){
            sprintf(line, "%d\n", int1+int2);
            n = strlen(line);
            if(write(STDOUT_FILENO, line, n) != n){
                perror("write");
                return -1;
            }
        }else{
            if(write(STDOUT_FILENO, "invalid args\n", 13) != 13){
                perror("write2");
                return -2;
            }
        }
    }

    return 0;
}
