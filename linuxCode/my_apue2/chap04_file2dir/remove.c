#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../lib/err_msg.h"

int
main(int argc, char *argv[])
{
    if(argc != 2)
        pr_msg2exit(1, "usage a.out path or dir\n");
    
    if(remove(argv[1]) != 0)
        pr_msg2exit(1, "remove eror[%s]\n", strerror(errno));

    return 0;
}


