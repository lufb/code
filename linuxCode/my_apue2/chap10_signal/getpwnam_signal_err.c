/*
    信号处理程序中调用了非可重入函数，后果是不可预料的。
*/
#include <pwd.h>
#include <stdio.h>
#include <signal.h>

static void
my_alarm(int signo)
{
    struct passwd           *rootptr;

    printf("in signal handler\n");
    if((rootptr = getpwnam("root")) == NULL)
    {
        perror("getpwnam");
    }
    alarm(1);
}

int
main(void)
{
    struct passwd   *ptr;

    signal(SIGALRM, my_alarm);
    alarm(1);
    while(1)
    {
        if((ptr = getpwnam("fuck")) == NULL)
        {
            perror("getpwnam2");
            continue;
        }
        if(strcmp(ptr->pw_name, "fuck") != 0)
        {
            printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
        }
    }

    return 0;
}
