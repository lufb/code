#ifndef _OPEN_H_
#define _OPEN_H_

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILE_MODE   (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int
_open(const char *name, int flag, int *err)
{
    int         fd, save_errno = errno;

    if((fd = open(name, flag)) < 0)
    {
        *err = errno;
        errno = save_errno;
        return -1;
    }

    return fd;
}

int
_open_w(const char *name,int *err)
{
    return _open(name, O_WRONLY, err);
}

int
_open_r(const char *name, int *err)
{
    return _open(name, O_RDONLY, err);
}

int
_open_rw(const char *name, int *err)
{
    return _open(name, O_RDWR, err);
}

int
_open_new(const char *name, int *err)
{
    int         fd, save_errno = errno;

    if((fd = open(name, O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0)
    {
        *err = errno;
        errno = save_errno;
        return -1;
    }

    return 0;
}

#endif

