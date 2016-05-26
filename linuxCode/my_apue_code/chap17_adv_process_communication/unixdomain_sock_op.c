/*
    练习使用UNIX域套接字的操作
*/

#include <sys/un.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>


#define  QLEN        10
#define  STALE       30
#define  CLI_PERM    S_IRWXU
#define  CLI_PATH    "/var/tmp/"

int
serv_listen(const char *name)
{
    int                         fd, len, err, rval;
    struct sockaddr_un          un;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        return -1;
    }

    unlink(name);

    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(name);

    if(bind(fd, (struct sockaddr *)&un, len) < 0){
        rval = -2;
        goto errout;
    }
    if(listen(fd, QLEN) < 0){
        rval = -3;
        goto errout;
    }
    return (fd);

errout:
    err = errno;
    close(fd);
    errno = err;
    return(rval);
}

int
serv_accept(int listenfd, uid_t *uidptr)
{
    int                     clifd, err, rval;
    socklen_t               len;
    time_t                  statetime;
    struct sockaddr_un      un;
    struct stat             statbuf;

    len = sizeof(un);
    if((clifd = accept(listenfd, (struct sockaddr *)&un, &len)) < 0){
        return -1;/*一般errno == EINTR */
    }

    len -= offsetof(struct sockaddr_un, sun_path);
    un.sun_path[len] = 0;

    if(stat(un.sun_path, &statbuf) < 0){
        rval = -2;
        goto errout;
    }

#ifdef      S_ISSOCK
    if(S_ISSOCK(statbuf.st_mode) == 0){ /*not a socket*/
        rval = -3;
        goto errout;
    }
#endif

    if((statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
        (statbuf.st_mode & S_IRWXU != S_IRWXU)){
        rval = -4;
        goto errout;
    }

    statetime = time(NULL) - STALE;
    if(statbuf.st_atime < statetime ||
        statbuf.st_ctime < statetime ||
        statbuf.st_mtime < statetime){
        rval = -5;
        goto errout;
    }

    if(uidptr != NULL){
        *uidptr = statbuf.st_uid;
    }

    unlink(un.sun_path);
    return (clifd);
errout:
    err = errno;
    close(clifd);
    errno = err;
    return (rval);
}


int
cli_conn(const char *name)
{
    int                 fd, len, err, rval;
    struct sockaddr_un  un;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        return -1;
    }

    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path, "%s%05d", CLI_PATH, getpid());
    len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

    unlink(un.sun_path);
    if(bind(fd, (struct sockaddr *)&un, len) < 0){
        rval = -2;
        goto errout;
    }
    if(chmod(un.sun_path, CLI_PERM) < 0){
        rval = -3;
        goto errout;
    }
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, name);
    len = len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
    if(connect(fd, (struct sockaddr *)&un, len) < 0){
        rval = -4;
        goto errout;
    }
errout:
    err = errno;
    close(fd);
    errno = err;
    return (rval); 
}
