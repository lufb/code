#include <stdio.h>

extern char** environ;  /*global environ*/

int main(void)
{
    int             i;

    for(i = 0; environ[i] != NULL; ++i)
        printf("environ[%d]: %s\n", i, environ[i]);

    return 0;
}
