#include <stdio.h>


void 
set()
{
    if(setuid(2) != 0)
        perror("setuid");
}

int
main()
{
	int		*p;
    while(1)
        sleep(1);

    set();
    
	*p = 1;

	return 0;
}
