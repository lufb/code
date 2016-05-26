#include "create_rand.h"
#include <time.h>
#include <stdlib.h>
#include <assert.h>

/*生成随机数[min, max)*/
int
creat_rand(int min, int max)
{
	assert(min <= max);
	int					offset = max-min;
	if(offset == 0)
		return min;
	
	return rand()%offset + min;
}

/*初始化随机数种子*/
void
init_rand()
{
	srand(time(NULL));
}