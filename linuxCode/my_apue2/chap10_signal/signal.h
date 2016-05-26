#include <signal.h>

typedef void Sigfunc(int);

/*
    ����ALRM�жϵĵ��ò�Ҫ�Զ�����
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
    ���б��жϵ�ϵͳ���ö����Զ�����
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
