#include <sys/time.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    struct timeval          t;

    if(gettimeofday(&t, NULL) == 0)
        printf("sec[%d]microseconds[%d]\n", t.tv_sec, t.tv_usec);

    time_t                  ti;
    //memset(&ti, 1, sizeof(ti));
    ti = ~0u>>1;
    
    printf("%s\n", ctime(&ti));
    return 0;
}
