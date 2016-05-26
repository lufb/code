#include <sys/time.h>
#include <stdio.h>
#include <time.h>


int main(void)
{
    int                 rc;
    struct timeval      tv_t;
    time_t              time_tt;
    struct tm           *ptm;
    char                *ptime;

    if((rc = gettimeofday(&tv_t, NULL)) != 0){    /* useconds*/
        perror("gettimeofday");

        return -1;
    }

    /* error: if(rc = time(&time_tt) == -1){ */
    /* may be time_tt would be rewriten */
    if(rc = time(&time_tt) == -1){              /* seconds */
        perror("time");

        return -2;
    }
    
    /* begin change */
    if((ptm = gmtime(&time_tt)) == NULL){    /* gmtime */
        perror("gmtime");

        return -3;
    }
    
    printf("after gmtime: hour[%d] min[%d] sec[%d]\n",
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    if((ptm = localtime(&time_tt)) == NULL){    /* localtime */
        perror("gmtime");

        return -3;
    }
    
    printf("after localtime: hour[%d] min[%d] sec[%d]\n",
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    time_tt = mktime(ptm);              /* mktime */
    printf("after mktime, seconds[%d]\n", time_tt);

    ptime = asctime(ptm);               /* asctime */
    printf("after asctime: %s", ptime);

    ptime = ctime(&time_tt);             /*ctime*/
    printf("after ctime: %s", ptime);

    char        formattime[256] = {0};
    strftime(formattime, sizeof(formattime),
        "%D %H %M %S", ptm);
    printf("after strftime: %s\n", formattime);
   
    return 0;
}   
