#include <signal.h>
#include <unistd.h>

static void
sig_alrm(int signo)
{

}


/***********************************
��sleep1����������: 
1:�����sleep1֮ǰ�����ӣ���Ὣ֮ǰ�����Ӹ��ǵ�
2:������SIGALRM�ź�
3:�����alarm����pause֮ǰ��ʱ�������������pause��
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
