#ifndef _ACCESS_H_
#define _ACCESS_H_

#include <unistd.h>

int
file_exist(const char *filename)
{
    if(access(filename, F_OK) < 0)
        return 0;
    
    return 1;
}


#endif
