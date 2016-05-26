/*
��ϰʹ��mmap
���������ļ�ӳ�䵽�ڴ棬���ڴ���в���
*/

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int
main(int argc, char *argv[])
{
    int             fdin, fdout;
    void            *src, *dst;
    struct stat     statbuf;

    if(argc != 3){
        printf("usage: %s <fromfile> <dstfile>\n", argv[0]);
        return -1;
    }

    if((fdin = open(argv[1], O_RDONLY)) < 0){
        printf("can't open [%s] for reading\n", argv[1]);
        return -2;
    }

    if((fdout = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0){
        printf("can't creat [%s] for writing\n", argv[2]);
        return -3;
    }

    if(fstat(fdin, &statbuf) < 0){
        printf("fstat [%s] error\n", argv[1]);
        return -4;
    }

    /*��������ļ��Ĵ�С����ʵ��������lseek�����ɿն�������*/
    /*���û�������ļ��Ĵ�С������"���ߴ���"����*/
    if(lseek(fdout, statbuf.st_size-1, SEEK_SET) == -1){
        printf("lseek [%s] error\n", argv[2]);
        return -5;
    }
    if(write(fdout, "", 1) != 1){
        printf("write [%s] error\n", argv[2]);
        return -6;
    }

    /*ӳ�������ļ����ڴ�*/
    if((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED){
        printf("mmap [%s] error\n", argv[1]);
        return -7;
    }
    if((dst = mmap(0, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, 0)) == MAP_FAILED){
        printf("mmap [%s] error\n", argv[2]);
        return -8;
    }

    /*ӳ���ֱ꣬�Ӳ����ڴ棬���cp����,ָ����MAP_SHARED���ں˻�����������д��������*/
    memcpy(dst, src, statbuf.st_size);

    return 0;    
}
