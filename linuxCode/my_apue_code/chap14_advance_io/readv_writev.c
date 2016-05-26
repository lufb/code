#include <sys/uio.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


void 
test_writev()
{
    char                psrc[64] = {0};
    const char  const   *psearch = ", ";
    char                *token = NULL;
    struct iovec        my_iovec[32];
    int                 i = 0;
    
    sprintf(psrc, "%s", "hell lufubo,this is a test");

    /*
    注意1 strtok要求传入的psrc必须有0节尾符，不然会有断错误
        2 strtok会修改psrc所指向的内存
    */
    token = strtok(psrc, psearch);
    while(token != NULL){
        //printf("size: %d %s\n", strlen(token), token);
        my_iovec[i].iov_base = token;
        my_iovec[i].iov_len = strlen(token);
        ++i;
        token = strtok(NULL, psearch);
    }

    ssize_t             writesize;
    writesize = writev(1, my_iovec, i);
}


int
create_file(char *pfile)
{
    int             fd;
    char            buf[] = "1234567890abcdefghijk\n1234567890abcdefghijk\n1234567890abcdefghijk\n1234567890abcdefghijk\n";

    if((fd = open(pfile, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0){
        perror("open");
        return -1;
    }

    if(write(fd, buf, sizeof(buf)) < 0){
        perror("write");
        return -2;
    }

    fsync(fd);

    return fd;
}

void
dele_file(char *pfile)
{
    if(unlink(pfile) < 0)
        perror("unlink");
}



void
test_readv()
{
    char                    buf0[2];
    char                    buf1[3];
    char                    buf2[4];
    struct iovec            my_iovec[3];
    ssize_t                 bytes_read;
    char                    *ptmpfile = "tmp123.txt";
    int                     fd;

    my_iovec[0].iov_base = buf0;
    my_iovec[0].iov_len= sizeof(buf0);
    my_iovec[1].iov_base = buf1;
    my_iovec[1].iov_len = sizeof(buf1);
    my_iovec[2].iov_base = buf2;
    my_iovec[2].iov_len = sizeof(buf2);

    if((fd = create_file(ptmpfile)) < 0)
        return;
    
    printf("begin test readv:\n");
    bytes_read = readv(fd, my_iovec, 3);/*这儿还有点问题，不晓得为什么总返回0*/
    printf("\n\nbytes_read[%d]\n", bytes_read);
    if(bytes_read < 0){
        perror("readv");
    }

    dele_file(ptmpfile);
    
}

int
main(void)
{
    test_writev();
    printf("\n\n");
    test_readv();
        
    return 0;
}
