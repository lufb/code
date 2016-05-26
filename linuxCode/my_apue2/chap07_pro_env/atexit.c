#include <stdlib.h>
#include "../lib/err_msg.h"

static void
my_exit1(void)
{
    printf("first exit handleer\n");
}

static void
my_exit2(void)
{
    printf("second exit handleer\n");
}

int
main(void)
{
    if(atexit(my_exit1) != 0)
        pr_msg2exit(1, "error1");
    if(atexit(my_exit2) != 0)
        pr_msg2exit(1, "error2");

    FILE            *fp;
    if((fp = fopen("tt.txt", "w+")) == NULL)
        pr_msg2exit(1, "error3");
    
    printf("main is done\n");
    
    sleep(30);
    
    _exit(0);
}
