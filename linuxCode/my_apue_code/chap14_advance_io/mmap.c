/*
练习使用mmap
即将磁盘文件映射到内存，对内存进行操作
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

    /*设置输出文件的大小，其实就是利用lseek可生成空洞的特性*/
    /*如果没有设置文件的大小，会有"总线错误"出现*/
    if(lseek(fdout, statbuf.st_size-1, SEEK_SET) == -1){
        printf("lseek [%s] error\n", argv[2]);
        return -5;
    }
    if(write(fdout, "", 1) != 1){
        printf("write [%s] error\n", argv[2]);
        return -6;
    }

    /*映射两个文件到内存*/
    if((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED){
        printf("mmap [%s] error\n", argv[1]);
        return -7;
    }
    if((dst = mmap(0, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, 0)) == MAP_FAILED){
        printf("mmap [%s] error\n", argv[2]);
        return -8;
    }

    /*映射完，直接操作内存，完成cp动作,指定的MAP_SHARED，内核会慢慢将数据写到磁盘上*/
    memcpy(dst, src, statbuf.st_size);

    return 0;    
}
