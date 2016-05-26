#ifndef _PREAD_H_
#define _PREAD_H_

#include <unistd.h>
#include <errno.h>
#include "open.h"
#include "../lib/err_msg.h"

int
_pwrite(int fd, const void *buf, size_t bytes, off_t offset, int *err)
{
    int             ret, save_errno = errno;
    
    if((ret = pwrite(fd, buf, bytes, offset)) == -1){
        *err  = errno;
        errno = save_errno;
        return -1;
    }
    
    return ret;
}

int
_pread(int fd, void *buf, size_t bytes, off_t offset, int *err)
{
    int            ret, save_errno = errno;

    if((ret = pread(fd, buf, bytes, offset)) == -1){
        *err  = errno;
        errno = save_errno;
        return -1;
    }

    return ret;
}
#endif
