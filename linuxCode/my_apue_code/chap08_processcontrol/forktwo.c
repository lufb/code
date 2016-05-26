#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/********************************************************/
/*  fork���������⽩������                              */
/*  �ڶ���forkʱ������һ���ӽ��������˳�                */
/*  ��init���������յڶ����ӽ��̣��������⽩ʬ����      */
/********************************************************/
int main(void)
{
    pid_t           pid;
    
    if((pid = fork()) < 0){
        perror("fork");
        return -1;
    }else if(pid == 0){    /* first child */
        if((pid = fork()) < 0){
            perror("fork");
            return -2;
        }else if(pid > 0){ /*�����ø������˳������ӽ��̵ĸ�����Ϊinit�����⽩ʬ���� */
            exit(0);
        }

        /*������������ĵڶ���fork�������ӽ��̣��ǲ����Ϊ��ʬ���̵�*/
        /* ��ȻҲҪ���丸�������˳���������sleep��ʵ�� */
        sleep(2);
        printf("second child, parent pid = %d\n", getppid());
        exit(0);
    }

    /* �ڸ������еȴ���һ���ӽ����˳� */
    /* ע�����ﻹ����pid */
    /*��Ϊ�ӽ��̻Ḵ�Ƹ����̿ռ䣬pid�������ø��� */
    sleep(1);
    if(waitpid(pid, NULL, 0) != pid){
        perror("waitpid");
        return -2;
    }

    return 0;
}