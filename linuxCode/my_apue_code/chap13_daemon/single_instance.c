/*
只允许启动一个副本 即一个程序只允许启动一遍
实现原理
    在进程启动时，创建一个特殊的文件，并在整个运行期都对这个文件加写锁
    之后再启动该程序的另一个副本时，会导致加锁不成功，打印出错信息

查看系统的很多后台进程都是这样来实现单实例的，比如sambd
*/

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <syslog.h>


#define LOCKFILE    "var/run/daemon.pid"    /*该程序需要创建的特殊文件名*/
#define LOCKMODE    (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

extern  int lockfile(int);


/*
return
    0:  还没运行
    <0: 出错
    >0: 已运行
*/
int
already_running(void)
{
    int                 fd;
    char                buf[16];

    fd = fopen(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    if(fd < 0){
        syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
        return -1;
    }

    if(lockfile(fd) < 0){
        if(errno == EACCES || errno == EAGAIN){/*已运行了*/
            close(fd);
            return 1;
        }
        syslog(LOG_ERR, "can't lock %s: %s", LOCKFILE, strerror(errno));
        return -2;
    }

    ftruncate(fd, 0);/*将文件长度截短为0*/
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);  /*将运行的进程号写到文件中*/
    return 0;   /**/
}
