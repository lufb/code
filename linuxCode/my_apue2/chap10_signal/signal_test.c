
/*1 下列代码存在有一个时间窗口，容易使信号丢失*/
void
sig_int()
{
    signal(SIGINT, sig_int);
}

/*2 下面代码容易死*/
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
