#include <stdio.h>
#include <bits/pthreadtypes.h>
#include <sys/types.h>

int
main()
{
    printf("%u\n", pthread_self());
    printf("%d\n", pthread_equal(pthread_self(),pthread_self()));

    return 0;
}
