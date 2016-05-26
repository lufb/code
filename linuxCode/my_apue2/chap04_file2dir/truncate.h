#ifndef _TRUCATE_H_
#define _TRUCATE_H_

#include <unistd.h>
#include <sys/types.h>  /*这里有off_t类型定义*/
#include <errno.h>

_truncate(const char *p, off_t length, int *err)
{
    int         save_errno = errno;

    if(truncate(p, length) != 0){
        *err = 0;
        errno = save_errno;

        return -1;
    }

    return 0;
}

#endif

