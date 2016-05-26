#ifndef _GET_CORE_SIZE_H_
#define _GET_CORE_SIZE_H_

#include <sys/resource.h>
#include <string.h>
#include <errno.h>
#include "../lib/err_msg.h"

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

