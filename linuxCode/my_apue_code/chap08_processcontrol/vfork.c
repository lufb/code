#include <unistd.h>
#include <sys/types.h>

int         glob = 6;

int main(void)
{
    int     val;
    pid_t   pid;

    val = 4;
    printf("before vfork\n");
    if((pid = vfork()) < 0){
        perror("vfork");

        return -1;
    }else if(pid == 0){          /* child */
    /* notice : before call exec */
    /* child will be run in parent's process space in using vfork */
    /* so below code will change parent's val and glob var */
        glob++;
        val++;
        _exit (0);              /* not to fluse IO*/
    }

    /* parent */
    printf("pid = %d, ppid = %d, glob = %d, var = %d\n", 
    getpid(), getppid(), glob, val);
}
