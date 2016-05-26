#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

/* 注意 在信号处理函数里面调用非可重入函数，结果是不可预料的 */
static void
my_alarm(int signo)
{
    struct passwd *rootptr;

    printf("in signal handler\n");
    if((rootptr = getpwnam("root")) == NULL){
        printf("error");
    }

    alarm(1);
}

int
main(void)
{
    struct passwd *ptr;

    signal(SIGALRM, my_alarm);
    alarm(1);
    for( ; ; ){
        if((ptr = getpwnam("lufubo")) == NULL){
            printf("error2");
            return -1;
        }

        if(strcmp(ptr->pw_name, "lufubo") != 0){
            printf("return value corrupted! pw_name = %s\n", ptr->pw_name);
        }
    }

    return 0;
}
