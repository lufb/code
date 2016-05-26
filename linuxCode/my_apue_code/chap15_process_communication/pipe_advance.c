/*
    �Ը�һЩ��pipeӦ��:���ļ����Ƶ��ֲ�����
*/
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DEF_PAGER   "/bin/more"

int
main(int argc, char *argv[])
{
    int                     n;
    int                     fd[2];
    pid_t                   pid;
    char                    *pager, *argv0;
    char                    line[1024];
    FILE                    *fp;

    if(argc != 2){
        printf("usage: a.out <pathname>\n");
        return -1;
    }

    if((fp = fopen(argv[1], "r")) == NULL){
        printf("open file [%s] error: %s\n", argv[1], strerror(errno));
        return -2;
    }
    
    if(pipe(fd) < 0){
        perror("pipe");
        return -3;
    }

    if((pid = fork()) < 0){
        perror("fork");
        return -4;
    }else if(pid > 0){
        close(fd[0]);
        while(fgets(line, sizeof(line), fp) != NULL){
            n = strlen(line);
            if(write(fd[1], line, n) != n){
                perror("write");
                return -5;
            }
        }
        if(ferror(fp)){ /*���ļ��������߶��ļ�����fgets���᷵��NULL,��ferror���ж��ǲ��Ƿ����˴���*/
            printf("read file [%s] error\n", argv[1]);
            return -6;
        }

        close(fd[1]);

        /*�ȴ��ӽ����˳�*/
        if(waitpid(pid, NULL, 0) < 0){
            perror("waitpid");
            return -7;
        }

        return 0;
    }else{
        close(fd[1]);
        /*Ҫ���ж��ǲ�����˲Ž����ļ�������*/
        if(fd[0] != STDIN_FILENO){
            if(dup2(fd[0], STDIN_FILENO) != STDIN_FILENO){
                perror("dup2");
                return -8;
            }
            close(fd[0]);/*Ҫ�رո��ļ�����������Ϊ�����Ƴ����ˣ������ļ����������Ժ󶼲�����*/
        }
        /*�ļ��������ѱ����ƣ��ӽ��̵ı�׼�����ǹܵ��ˣ�������ǰ��fd[0]*/
        /*��ȡҳ�Ĵ�С*/
        if((pager = getenv("PAGER")) == NULL)
            pager = DEF_PAGER;
        if((argv0 = strrchr(pager, '/')) != NULL)
            argv0++;
        else
            argv0 = pager;

        if(execl(pager, argv0, (char *)0) < 0){
            perror("execl");
            return -9;
        }
        return 0;
    }
    
}
