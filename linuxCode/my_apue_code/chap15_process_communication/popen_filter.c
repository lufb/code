/*
    �˳�������popenȥ����һ��filter�Ĺ��˳���filter��filter.c�������
    popen�������ڵ��ù��˳���
*/

#include <sys/wait.h>
#include <stdio.h>

#define FILTER              "./filter"    /*���õĿɹҿ��Ĺ��˳�������,�ڴ˲��Գ���������filter.c�������ɵ�*/

int
main(void)
{
    char                    line[1024];
    FILE                    *fpin;

    if((fpin = popen(FILTER, "r")) == NULL){/*��fpin���ӵ�FILTER�ı�׼���*/
        perror("popen");
        return -1;
    }

    for( ; ; ){
        fputs("prompt> ", stdout);
        fflush(stdout);
        if(fgets(line, sizeof(line), fpin) == NULL)/*�ӹܵ����*/
            break;
        if(fputs(line, stdout) == EOF){
            perror("fputs");
            return -2;
        }
    }

    if(pclose(fpin) == -1){
        perror("pclose");
        return -3;
    }
    putchar('\n');
    return 0;
}
