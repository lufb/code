#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "./pr_exit.h"


static void
pr_times(clock_t real, struct tms *tms_start, struct tms *tms_end)
{
    static long     clkck = 0;

    if(clkck == 0)
    {
        if((clkck = sysconf(_SC_CLK_TCK)) < 0)
        {
            perror("sysconf");
            exit(1);
        }
    }
    printf("real: %7.2f\n", real/(double)clkck);
    printf("user: %7.2f\n",
           (tms_end->tms_utime - tms_start->tms_utime)/(double)clkck);
    printf("sys: %7.2f\n",
           (tms_end->tms_stime - tms_start->tms_stime)/(double)clkck);
    printf("child user: %7.2f\n",
           (tms_end->tms_cutime - tms_start->tms_cutime)/(double)clkck);
    printf("child sys: %7.2f\n",
          (tms_end->tms_cstime - tms_start->tms_cstime)/(double)clkck);

}

static void
do_cmd(char *cmd)
{
    struct tms      tms_start, tms_end;
    clock_t         start, end;
    int             status;

    printf("\ncommand:%s\n", cmd);
    if((start = times(&tms_start)) == -1)
    {
        perror("times");
        exit(1);
    }
    if((status = system(cmd)) < 0)
    {
        printf("system error\n");
        exit(1);
    }
    if((end = times(&tms_end)) == -1)
    {
        perror("times2");
        exit(1);
    }
    pr_times(end-start, &tms_start, &tms_end);
    pr_exit(status);
}

int
main(int argc, char *argv[])
{
    int         i;

    setbuf(stdout, NULL);
    for(i = 1; i < argc; ++i)
        do_cmd(argv[i]);

    exit(0);
}
