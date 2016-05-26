#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


static void
charatatime(char *str)
{
    char        *ptr;
    int         c;

    setbuf(stdout, NULL);
    for(ptr = str; (c = *ptr++) != 0; )
        putc(c, stdout);
}

int
main(void)
{
    pid_t               pid;

    TELL_WAIT();        //add
    
    if((pid = fork()) < 0)
    {
        perror("fork");
        exit(1);
    }
    else if(pid == 0)
    {
        WAIT_PARENT();//add
        charatatime("output from child\n");
    }
    else
    {
        charatatime("output form parent\n");
        TELL_CHILD(pid);//add
    }

    exit(0);
}
