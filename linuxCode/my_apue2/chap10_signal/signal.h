#include <signal.h>

typedef void Sigfunc(int);

/*
    仅被ALRM中断的调用不要自动重启
*/
Sigfunc*
_signal(int signo, Sigfunc *func)
{
    struct sigaction        act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(signo == SIGALRM)
    {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    }
    else
    {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }

    if(sigaction(signo, &act, &oact) < 0)
        return (SIG_ERR);

    return oact.sa_handler;
}

/*
    所有被中断的系统调用都不自动重启
*/
Sigfunc*
signal_intr(int signo, Sigfunc *func)
{
    struct sigaction        act, oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
    if(sigaction(signo, &act, &oact) < 0)
        return (SIG_ERR);
    return oact.sa_handler;
}
