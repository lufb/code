#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../lib/err_msg.h"

int
main(int argc, char *argv[])
{
    if(argc != 2)
        pr_msg2exit(1, "usage a.out path\n");
    
    if(unlink(argv[1]) != 0)
        pr_msg2exit(1, "unlink eror[%s]\n", strerror(errno));

    return 0;
}

