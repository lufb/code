#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../lib/err_msg.h"

int
main(int argc, char *argv[])
{
    if(argc != 3)
        pr_msg2exit(1, "usage a.out path, new path\n");
    
    if(link(argv[1], argv[2]) != 0)
        pr_msg2exit(1, "link eror[%s]\n", strerror(errno));

    return 0;
}
