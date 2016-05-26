/*
   ʹ��popen��дpipe_advance.c
   popen��ʵ��ȫ����pipeʵ�֣�ֻ���������Ѳ�������,����д����:
    �������ܵ�=>����fork��������=>�رղ�ʹ�õ�һ��=>ִ��shell���Ȼ��ȴ���ֹ
*/

#include <sys/wait.h>
#include <stdio.h>

/*
shell����${PAGER:-more}��˼
���PAGER���ڣ���ֵ�ǿգ���ʹ��PAGERֵ������ʹ��more����
*/
#define PAGER                   "${PAGER:-more}"

int 
main(int argc, char *argv[])
{
    char                line[1024];
    FILE                *fpin, *fpout;

    if(argc != 2){
        printf("usage: a.out <pathname>");
        return -1;
    }

    if((fpin = fopen(argv[1], "r")) == NULL){   /*��,ע�������Ǵ��ļ�*/
        perror("fopen fpin");
        return -2;
    }
    if((fpout = popen(PAGER, "w")) == NULL){    /*w���ӵ�PAGER�ı�׼���룬��֮����ʵִ����fork,dup, wait����*/
        perror("popen fpout");
        return -3;
    }

    while(fgets(line, sizeof(line), fpin) != NULL){
        if(fputs(line, fpout) == EOF){
            perror("fputs");
            return -4;
        }
    }
    if(ferror(fpin)){
        perror("fgets");
        return -5;
    }
    if(pclose(fpout) == -1){
        perror("fclose");
        return -6;
    }

    return 0;
}
