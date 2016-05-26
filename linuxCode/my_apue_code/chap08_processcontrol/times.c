#include <sys/times.h>
#include <stdio.h>
#include <unistd.h>

static void
pr_times(clock_t real, struct tms *tmstart, struct tms *tmsend)
{
    static long clktck = 0;

    if(clktck == 0){
        if((clktck = sysconf(_SC_CLK_TCK)) < 0){
            printf("sycconf() error");
        }
    }

    printf("    real:   %7.2f\n", real/(double)clktck);
    printf("    user:   %7.2f\n", (tmsend->tms_cutime - tmstart->tms_cutime)/(double)clktck);
    printf("    sys:    %7.2f\n", (tmsend->tms_stime- tmstart->tms_stime)/(double)clktck);
}

static void
do_cmd(char *cmdstring)
{
    struct tms          tmsstart, tmsend;
    clock_t             start, end;
    int                 status;

    if((start = times(&tmsstart)) == -1){
        perror("times");
        return ;
    }

    if((status = system(cmdstring)) < 0){
        perror("system()");
        return ;
    }

    if((end = times(&tmsend)) == -1){
        perror("times");
        return ;
    }

    pr_times(end-start, &tmsstart, &tmsend);
}

int main(int argc, char *argv[])
{
    int         i;

    setbuf(stdout, NULL);
    for(i = 1; i < argc; ++i)
        do_cmd(argv[i]);

    return 0;
}
