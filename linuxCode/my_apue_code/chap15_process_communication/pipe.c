/*
    �ó�����ϰʹ�ùܵ������������ӽ��̷�����
    �ܵ�ȱ��:   (1)��Щϵͳ�ܵ��ǰ�˫���ģ�
                (2)���ʺ��������̼�ͨ��
    ����:       (1)����һ��д���ѹرյĹܵ�ʱ�����������ݶ�����󣬷���0
                (2)��дһ�������ѹرյĹܵ�ʱ������SIGPIPE�ź�
                (3)��д�ܵ�ʱ������������ͬһ�ܵ�дС��BUF�����ݣ�������ң������д����BUF,����໥���壬������
*/

#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int 
main(int argc, char *argv[])
{
    int                     n;
    int                     fd[2];
    pid_t                   pid;
    char                    line[1024];

    if(pipe(fd) < 0){ /*�����ܵ���fd[0]���ڶ�,fd[1]����д*/
        perror("pipe");
        return -1;
    }

    if((pid = fork()) < 0){
        perror("fork");
        return -2;
    }else if(pid > 0){
        close(fd[0]);
        write(fd[1], "hello world\n", 12);
    }else{
        close(fd[1]);
        n = read(fd[0], line, sizeof(line));
        write(STDOUT_FILENO, line, n);
    }

    return 0;
}
