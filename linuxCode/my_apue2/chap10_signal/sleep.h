#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

static void
sig_alrm(int signo)
{

}

unsigned int
sleep1(unsigned int nsecs)
{
    if(signal(SIGALRM, sig_alrm) == SIG_ERR)
        return nsecs;
    alarm(nsecs);
    pause();

    return (alarm(0));
}

//
static jmp_buf env_alrm;

static void
sig_alrm2(int signo)
{
    longjmp(env_alrm, 1);
}

unsigned int
sleep2(int nsecs)
{
    if(signal(SIGALRM, sig_alrm2) == SIG_ERR)
        return nsecs;

    if(setjmp(env_alrm) == 0)
    {
        alarm(nsecs);
        pause();
    }
    return (alarm(0));
}
