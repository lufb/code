/*
   ��һ����ַ��__unix���׽���
*/

#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <stdio.h>

int
main(void)
{
    int                         fd, size;
    struct sockaddr_un          un;
    const char                  *foo_sock = "foo.socket";

    unlink(foo_sock);
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, foo_sock);
    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }
    /*Ҫ��ƫ��������Ϊ��Щϵͳ����Ľṹ��һ��*/
    size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
    if(bind(fd, (struct sockaddr *)&un, size) < 0){
        perror("bind");
        return -2;
    }

    printf("UNIX domain socket bonund\n");
    return 0;
}
