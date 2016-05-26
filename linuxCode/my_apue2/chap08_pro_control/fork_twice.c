#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int
main(void)
{
    pid_t               pid;

    if((pid = fork()) < 0)
    {
        perror("fork1");
        exit(1);
    }
    else if(pid == 0)
    {
        if((pid = fork()) < 0)
        {
            perror("fork2");
            exit(1);
        }else if(pid > 0)
            exit(0);
        sleep(200);//˯һ��ʱ�䣬��֤�丸�����˳����ø�������Ϊ��������ʬ���Լ���ʬ����init����
        printf("second child, parent pid = %d\n", getppid());
        exit(0);
    }

    if(waitpid(pid, NULL, 0) != pid)
    {
        perror("waitpid");
        exit(1);
    }

    exit(0);
}


