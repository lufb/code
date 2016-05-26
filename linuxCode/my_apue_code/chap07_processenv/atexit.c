#include <stdlib.h>
#include <stdio.h>


static void my_exit1(void);
static void my_exit2(void);

int main(int argc, char *argv[])
{
    if(atexit(my_exit1) != 0){      /* register my_exit1 */
        printf("atexit my_exit1 error\n");

        return -1;
    }
    printf("atexit my_exit1 sucess\n");

    if(atexit(my_exit2) != 0){      /* register my_exit2 */
        printf("atexit my_exit2 error\n");

        return -2;
    }
    printf("atexit my_exit2 sucess\n");

    printf("main had done\n");

    exit(0);/* will call the register funcs */

    /*****************************************************/
    /* only below condition will not calling atexit funcs*/
    /*  condition1: _exit(0)                             */
    /*  condition2: _Exit(0)                             */
    /*****************************************************/   
}

static void my_exit1(void)
{
    printf("execute exit1\n");
}

static void my_exit2(void)
{
    printf("execute exit2\n");
}
