#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    pid_t           pid;

    if((pid = fork()) < 0){
        perror("fork");
        return -1;
    }else if(pid == 0){
        if(execlp("echoall", "echoall1", "only 1 param", (char *)0) < 0){
            perror("execlp");
            return 1;
        }
    }

    if(waitpid(pid, NULL, 0) != pid){
        perror("waitpid");
        return -2;
    }else{
        printf("wait execlp return sucess\n");
        return 0;
    }
}
