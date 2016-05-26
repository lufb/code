#include <setjmp.h>
#include <errno.h>
#include <string.h>
#include "../lib/err_msg.h"

jmp_buf         jmpbuffer;
int             i = 0;

void
func1()
{
    longjmp(jmpbuffer, ++i);
}

void
func2()
{
    func1();
}


int
main(void)
{
    int             rc;
    
    if((rc = setjmp(jmpbuffer)) != 0)
        printf("error [%d]\n", rc);

    func2(2);
    
    exit(0);
}
