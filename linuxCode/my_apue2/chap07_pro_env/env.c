#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

extern char **environ;

int
main(int argc, char *argv[])
{
    int             i = 0;
    
    while(environ[i])
        printf("[%d: %s]\n", i, environ[i++]);

    printf("\n\n%s\n", getenv("USER"));

    if(putenv("USER=agayibu"))
        printf("Error\n");
    
    if(unsetenv("USER"))
        printf("Error2\n");
    i = 0;
    while(environ[i])
        printf("[%d: %s]\n", i, environ[i++]);
    
    return 0;
}
