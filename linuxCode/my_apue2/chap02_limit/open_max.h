#ifndef _OPEN_MAX_H_
#define _OPEN_MAX_H_

#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>
#include "../lib/err_msg.h"

#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

#define OPEN_MAX_GUESS 256

long
open_max(void)
{
    if(openmax == 0)
    {
        errno = 0;
        if((openmax = sysconf(_SC_OPEN_MAX)) < 0)
        {
            openmax = OPEN_MAX_GUESS;
        }
    }

    return openmax;
}

long
get_core_size()
{
    struct rlimit           limit;

    if(getrlimit(RLIMIT_CORE, &limit))
    {
        pr_msg("getrlimit error[%s]\n", strerror(errno));
        return -errno;
    }

    return limit.rlim_max;
}
#endif

