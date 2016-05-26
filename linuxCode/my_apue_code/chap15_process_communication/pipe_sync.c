/*
    利用管道的特点来实现两个进程的同步
实现原理如下:

    父进程                                  子进程
   |--------|             "p"             |---------|         
   | pfd1[1]| --------------------------> | pfd1[0] |
   |        |             "c"             |         |
   | pfd2[0]| <-------------------------- | pfd2[1] |
   |________|                             |_________|                  
*/
#include <sys/types.h>


static int          pfd1[2], pfd2[2];

void
TELL_WAIT(void)
{
    if(pipe(pfd1) < 0 || pipe(pfd2) < 0)
        perror("pipe");
}

void
TELL_PARENT(pid_t pid)
{
    if(write(pfd2[1], "c", 1) < 0)
        perror("write in tell_parent");
}

void
WAIT_PARENT()
{
    char            c;
    
    if(read(pfd1[0], &c, 1) != 1){
        perror("read in wait_parent");
        return;
    }
    if(c != 'p'){
        printf("error in wait_parent\n");
    }
}

void
TELL_CHILD(pid_t pid)
{
    if(write(pfd1[1], "p", 1) < 0)
        perror("write in tell_parent");
}

void
WAIT_CHILD()
{
    char        c;

    if(read(pfd2[0], &c, 1) != 1){
        perror("read in wait_child");
        return;
    }
    if(c != 'c')
        printf("error in wait_child");
}
