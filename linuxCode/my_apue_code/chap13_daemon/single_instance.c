/*
ֻ��������һ������ ��һ������ֻ��������һ��
ʵ��ԭ��
    �ڽ�������ʱ������һ��������ļ����������������ڶ�������ļ���д��
    ֮���������ó������һ������ʱ���ᵼ�¼������ɹ�����ӡ������Ϣ

�鿴ϵͳ�ĺܶ��̨���̶���������ʵ�ֵ�ʵ���ģ�����sambd
*/

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <syslog.h>


#define LOCKFILE    "var/run/daemon.pid"    /*�ó�����Ҫ�����������ļ���*/
#define LOCKMODE    (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

extern  int lockfile(int);


/*
return
    0:  ��û����
    <0: ����
    >0: ������
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
        if(errno == EACCES || errno == EAGAIN){/*��������*/
            close(fd);
            return 1;
        }
        syslog(LOG_ERR, "can't lock %s: %s", LOCKFILE, strerror(errno));
        return -2;
    }

    ftruncate(fd, 0);/*���ļ����Ƚض�Ϊ0*/
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf)+1);  /*�����еĽ��̺�д���ļ���*/
    return 0;   /**/
}
