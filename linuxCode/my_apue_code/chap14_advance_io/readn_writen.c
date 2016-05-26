#include <stdio.h>


/*返回已读的字节数*/
size_t
my_readn(int fd, void *ptr, size_t n)
{
    size_t              nleft;
    size_t              nread;

    nleft = n;
    while(nleft > 0){
        if((nread = read(fd, ptr, nleft)) < 0){
            if(nleft == n)
                return -1;
            else
                break;
        }else if(nread == 0){
            break;
        }
        nleft -= nread;
        ptr += nread;
    }

    return (n - nleft);
}

/*返回已写的字节数*/
size_t
my_writen(int fd, void *ptr, size_t n)
{
    size_t              nleft;
    size_t              nwrite;

    nleft = n;
    while(nleft > 0){
        if((nwrite = write(fd, ptr, nleft)) < 0){
            if(nleft == n)
                return -1;
            else
                break;
        }else if(nwrite == 0){
            break;
        }
        nleft -= nwrite;
        ptr   += nwrite;
    }

    return (n - nleft);
}
