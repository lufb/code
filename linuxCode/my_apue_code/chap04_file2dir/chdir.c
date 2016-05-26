#include <unistd.h>
#include <stdio.h>

#define MAX_PATH    256
int main(void)
{
    if(chdir("/tmp") < 0){
        perror("chdir");

        return -1;
    }

    char            curdir[MAX_PATH] = {0};
    char            *ptr;

    if((ptr = getcwd(curdir, MAX_PATH)) == NULL){
        perror("getcwd");

        return -2;
    }

    printf("curent directory: %s\n", curdir);
    printf("curent directory: %s\n", ptr);

    return 0;
}
