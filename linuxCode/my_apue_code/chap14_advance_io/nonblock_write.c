#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/*open flags*/
void
set_f1(int fd, int flags)
{
    int         val;

    if((val = fcntl(fd, F_GETFL, 0)) < 0){
        printf("fcntl error\n");
        return;
    }

    val |= flags;

    if(fcntl(fd, F_SETFL, val) < 0){
        printf("fcntl error");
    }
}

/*close flags*/
void
clr_f1(int fd, int flags)
{
    int         val;

    if((val = fcntl(fd, F_GETFL, 0)) < 0){
        printf("fcntl error\n");
        return;
    }

    val &=~flags;

    if(fcntl(fd, F_SETFL, val) < 0){
        printf("fcntl error");
    }
}

char buffer[500000];

int
main(void)
{
    int             ntowrite, nwrite;
    char            *ptr;

    ntowrite = read(STDIN_FILENO, buffer, sizeof(buffer));
    fprintf(stderr, "read %d bytes\n", ntowrite);

    set_f1(STDIN_FILENO, O_NONBLOCK);   /*ÉèÖÃ·Ç×èÈû*/

    ptr = buffer;
    while(ntowrite > 0){
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

        if(nwrite > 0){
            ptr += nwrite;
            ntowrite -= nwrite;
        }else{
            fprintf(stderr, "error: %s\n", strerror(errno));
        }
    }

    clr_f1(STDOUT_FILENO, O_NONBLOCK);

    return 0;
}
