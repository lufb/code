#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

static jmp_buf      env_alrm;

static void
sig_alrm(int signo)
{
    longjmp(env_alrm, 1);/* 利用这点跳转到指定位置 */
}


/***********************************
此sleep2解决sleep1的第三个问题，但1，2没解决: 
即如果在alarm调用pause之前超时，也会返回
***********************************/
unsigned int
sleep2(unsigned int nsecs)
{
    if(signal(SIGALRM, sig_alrm) == SIG_ERR){
        return (nsecs);
    }

    if(setjmp(env_alrm) == 0){
        alarm(nsecs);
        pause();
    }

    return (alarm(0));
}
