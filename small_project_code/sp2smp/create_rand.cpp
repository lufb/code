#include "create_rand.h"
#include <time.h>
#include <stdlib.h>
#include <assert.h>

/*���������[min, max)*/
int
creat_rand(int min, int max)
{
	assert(min <= max);
	int					offset = max-min;
	if(offset == 0)
		return min;
	
	return rand()%offset + min;
}

/*��ʼ�����������*/
void
init_rand()
{
	srand(time(NULL));
}