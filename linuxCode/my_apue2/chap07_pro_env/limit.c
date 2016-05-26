#include <sys/resource.h>
#include <errno.h>
#include <string.h>

#define ULIMIT      1       /*1: can core dump  0: can't core dump*/          
#define doit(name)  pr_limits(#name, name)

static void
pr_limits(char *name, int resource)
{
    struct rlimit           limit;

    if(getrlimit(resource, &limit) < 0)
        printf("getrlimit error[%s]\n", strerror(errno));

    printf("%-14s ", name);
    if(limit.rlim_cur == RLIM_INFINITY)
        printf("(infinite)");
    else
        printf("%10d", limit.rlim_cur);
    
    putchar('\n');
}

/*set can't core dump*/
void 
set0()
{
   struct rlimit           limit;

   getrlimit(RLIMIT_CORE, &limit);
   limit.rlim_cur = 0;

   if(setrlimit(RLIMIT_CORE, &limit) < 0)
        printf("setrlimit1 error [%s]\n", strerror(errno));
}

/*set can core dump*/
void
set1()
{
   struct rlimit           limit;

   getrlimit(RLIMIT_CORE, &limit);
   limit.rlim_cur = RLIM_INFINITY;

   if(setrlimit(RLIMIT_CORE, &limit) < 0)
        printf("setrlimit1 error\n", strerror(errno));
}

int
main(void)
{
    printf("before\n");
    doit(RLIMIT_CORE);
    
    if(ULIMIT)
        set1();
    else
        set0();
    
    printf("after\n");
    doit(RLIMIT_CORE);
    
    int         *a;
    *a = 4;                 /* test core dump*/
        
    return 0;
}
