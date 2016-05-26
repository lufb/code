#include <signal.h>
#include <unistd.h>

static void
sig_alrm(int signo)
{

}


/***********************************
此sleep1的三个问题: 
1:如果在sleep1之前有闹钟，则会将之前的闹钟覆盖掉
2:重置了SIGALRM信号
3:如果在alarm调用pause之前超时，则会永阻塞在pause处
***********************************/
unsigned int
sleep1(int nsecs)
{
    if(signal(SIGALRM, sig_alrm) == SIG_ERR){
        return nsecs;
    }

    alarm(nsecs);
    pause();

    return (alarm(0));
}
