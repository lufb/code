#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../lib/err_msg.h"

#define MAXLINE     1024

int
main(void)
{
    char                name[L_tmpnam], line[MAXLINE];
    FILE                *fp;
    
    printf("%s\n", tmpnam(NULL));
    tmpnam(name);
    printf("%x\n", name);

    if((fp = tmpfile()) == NULL)
        pr_msg2exit(1, "tmpfile error [%s]\n", strerror(errno));

    fputs("onl line of output \n", fp);
    rewind(fp);
    if(fgets(line, sizeof(line), fp) == NULL)
        pr_msg2exit(1, "gets error[%s]\n", strerror(errno));
    fputs(line, stdout);

    return 0;
}
