#include <grp.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    struct group        *pgrp;

    if((pgrp = getgrnam(argv[1])) != NULL)
        {
        printf("group_name[%s] gid[%d]\n", argv[1], pgrp->gr_gid);
    }

    return 0;
}
