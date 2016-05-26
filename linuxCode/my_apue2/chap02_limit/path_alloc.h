#ifndef _PATH_ALLOC_H_
#define _PATH_ALLOC_H_

#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include "../lib/err_msg.h"

static int pathmax = 0;

char *
path_alloc(int *sizep)
{
    char            *ptr;

    if(pathmax == 0)
    {
        if((pathmax = pathconf("/", _PC_PATH_MAX)) < 0)
        {
            pr_msg("pathconf error [%s]\n", strerror(errno));
            return NULL;
        }
        ++pathmax;  /* 1×Ö½ÚµÄ'/' */
    }

    if((ptr = (char *)malloc(pathmax)) == NULL)
    {
        pr_msg("malloc error [%s]\n", strerror(errno));
        return NULL;
    }

    *sizep = pathmax;

    return ptr;
}

#endif

