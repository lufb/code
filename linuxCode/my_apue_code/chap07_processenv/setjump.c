#include <setjmp.h>

jmp_buf jmpbuffer;

int func()
{
    int         tmp = 0;
    //jump
    longjmp(jmpbuffer, 2);

    tmp = 3;
    return tmp;
}
int main(int argc, char *argv[])
{
    int             tmp = 0;
    volatile int    i = 0;
    int             ret;
    
    if((ret = setjmp(jmpbuffer)) != 0){
        printf("setjmp error[%d]\n", ret);
        return ret;
    }

        
    ++tmp;
    for(; i < 5; ++i)
        func();

    return 0;
}
