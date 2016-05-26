#include "chap03_fileIO/open.h"
#include <string.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    int             err,fd;

    if((fd = _open_new(argv[1], &err)) < 0){
        printf("open [%s] error[%s]\n", argv[1], strerror(err));
    }

    return 0;
}
