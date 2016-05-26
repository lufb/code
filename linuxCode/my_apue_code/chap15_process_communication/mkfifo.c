/*
    ��ϰʹ�������ܵ�:
    ������:
    (1) �����ݴ�һ���ܵ����͵���һ����Ϊ�˲���Ҫ�������м���ʱ�ļ�
    (2) �ڿͻ������������֮����д�������
*/
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

/* (1) �����ݴ��͵���һ���ܵ� */
void
fifo_trans()
{
    const char              tmpname[] = "/tmp/tmp_lufubo.fifo";
    pid_t                   pid;
    int                     fd;

    unlink(tmpname);
    if(mkfifo(tmpname, 0600) < 0){
            perror("mkfifo");
            exit(1);
    }
    

    if((pid = fork()) < 0){
        perror("fork");
        exit(1);
    }else if(pid > 0){
        char            tmp[] = "hello world";

        fd = open(tmpname, O_WRONLY );
        write(fd, tmp, sizeof(tmp));
    }else{
        char            tmp[1024];
        
        fd = open(tmpname, O_RDONLY);
        read(fd, tmp, sizeof(tmp));
        printf("the msg from fifo: %s\n", tmp);
    }
}

/*(2) �ͻ������������֮��������ݴ���*/
void
fifo_cli2srv()
{

}

int
main(void)
{
    fifo_trans();
    fifo_cli2srv();

    return 0;
}
