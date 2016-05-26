#include <unistd.h>
#include <sys/types.h>

int     glob = 6;
char    buf[] = "a write to stdout\n";

int main(void)
{
    int     var;
    pid_t   pid;

    var = 88;
    if(write(STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof(buf)-1){
        perror("write");

        return -1;
    }

    printf("before fork\n");

    if((pid = fork()) < 0){
        perror("fork");

        return -2;
    }else if(pid == 0){     /* child */
        ++glob;
        ++var;
    }else{                  /* parent */
        sleep(2);
    }

    printf("pid = %d, ppid = %d, glob = %d, var = %d\n", getpid(), getppid(), glob, var);

    return 0;
}
