
/*1 ���д��������һ��ʱ�䴰�ڣ�����ʹ�źŶ�ʧ*/
void
sig_int()
{
    signal(SIGINT, sig_int);
}

/*2 �������������*/
int sig_int_flag;
main()
{
    signal(SIGINT, sig_int);
    while(sig_int_flag == 0)
        pause();
}
sig_int()
{
    signal(SIGINT,sig_int);
    sig_int_flag = 1;
}
