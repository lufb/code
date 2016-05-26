#ifndef _FCNTL_SET_H_
#define _FCNTL_SET_H_

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../lib/err_msg.h"

void
set_f1(int fd, int flags)
{
    int             val;

    if((val = fcntl(fd, F_GETFL, 0)) < 0)
    {
        pr_msg("fcntl F_GETFL [%d] error[%s]\n", fd, strerror(errno));
        return;
    }

    val |= flags;   /*  ´ò¿ª    */

    if(fcntl(fd, F_SETFL, val) < 0)
        pr_msg("fcntl F_SETFL [%d] error [%s]\n", fd, strerror(errno));
}

void
clr_f1(int fd, int flags)
{
    int             val;

    if((val = fcntl(fd, F_GETFL, 0)) < 0)
    {
        pr_msg("fcntl F_GETFL [%d] error[%s]\n", fd, strerror(errno));
        return;
    }

    val &= ~flags;  /*  ¹Ø±Õ    */

    if(fcntl(fd, F_SETFL, val) < 0)
        pr_msg("fcntl F_SETFL [%d] error [%s]\n", fd, strerror(errno));
}

#endif
