/*
    �ó�����ϰʹ��select��poll����
    select������:
        fd_set  fdset
        FD_SET(int fd, &fdset);
        ��������У�fdset��ʵ�Ǹ����飬
        ����Ҫ������ӵ�fdset�е�Ԫ�ظ���С��FD_SETSIZE;
        ��Ȼ������ڴ�Խ�緢����
    select��ȱ��:
        Ч�ʲ��ߣ���������׽��ֹ���ʱ����ͻ��
        ��FD_SETSIZE������
    
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

/*�õ��ܴ򿪵�����ļ�������*/
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

/*�õ�ϵͳ��FD_SETSIZE��С*/
unsigned int get_fd_setsize()
{
    return FD_SETSIZE;
}

/*��ϰʹ��select*/
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
    for(i = 0; i < maxfd+10; ++i){   /*���Խ�� */
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


