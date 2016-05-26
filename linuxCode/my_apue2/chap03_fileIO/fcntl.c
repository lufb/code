#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "../lib/err_msg.h"


int
main(int argc, char *argv[])
{
    int             val;

    if(argc != 2)
        pr_msg2exit(1, "usage :a.out <descriptor#>\n");

    if((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
        pr_msg2exit(1, "fcntl [%d] error [%s]\n", atoi(argv[1]), strerror(errno));
    printf("filno[%d]:", atoi(argv[1]));
    switch(val & O_ACCMODE)
    {
        case O_RDONLY:
            printf("read only");
            break;
        case O_WRONLY:
            printf("write only");
            break;
        case O_RDWR:
            printf("read write");
            break;
        default:
            pr_msg2exit(1, "unknown access mode");
    }

    if(val & O_APPEND)
        printf(", append");
    if(val & O_NONBLOCK)
        printf(", nonblock");
#if defined(O_SYNC)
    if(val & O_SYNC)
        printf(", synchronous writes");
#endif
#if !defined(_POSIX_C_SOURCE)&& defined(O_FSYNC)
    if(val & O_FSYNC)
        printf(", synchronous writes");
#endif
    putchar('\n');
    exit(0);
}
