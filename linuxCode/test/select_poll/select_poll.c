/*
    该程序练习使用select及poll函数
    select的限制:
        fd_set  fdset
        FD_SET(int fd, &fdset);
        上面代码中，fdset其实是个数组，
        必须要满足添加到fdset中的元素个数小于FD_SETSIZE;
        不然会会有内存越界发生。
    select的缺点:
        效率不高，当管理的套接字过多时尤其突出
        受FD_SETSIZE的限制
    
*/
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#ifdef OPEN_MAX
static int  openmax = OPEN_MAX;
#else
static int  openmax = 0;
#endif

#define OPEN_MAX_GUESS  1024

/*得到能打开的最大文件描述符*/
int
open_max(void)
{
    if(openmax == 0){
        errno = 0;
        if((openmax = sysconf(_SC_OPEN_MAX)) < 0){
            if(errno == 0)
                openmax = OPEN_MAX_GUESS;
            else
                perror("sysconf error for _SC_OPEN_MAX");
        }
    }

    return openmax;
}

/*得到系统中FD_SETSIZE大小*/
unsigned int get_fd_setsize()
{
    return FD_SETSIZE;
}

/*练习使用select*/
void
use_select()
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1)/*error*/
        perror("select()");
    else if (retval)/*data come*/
        printf("Data is available now.\n");
    /* FD_ISSET(0, &rfds) will be true. */
    else/* timeout */
        printf("No data within five seconds.\n");    
}

void
danger_use_select()
{
    unsigned int                 maxfd  = get_fd_setsize();
    unsigned int                 i;
    fd_set                       rfds;


    FD_ZERO(&rfds);
    for(i = 0; i < maxfd+10; ++i){   /*这儿越界 */
        FD_SET(i, &rfds);
    }
    
}


int
main(void)
{
    printf("fd_setsize: %u\n", get_fd_setsize());

    //use_select();
    //danger_use_select();
    printf("maxfd:%d\n", open_max());
    
    return 0;
}


