#include "priority_test.h"
#include "priority.h"
#include <stdio.h>

int
test_pri()
{
	MPriority		pri;

	int				i,tmp, ret;
	for(i = 0; i < PRI_LIST_SIZE*2+1; ++i){
		ret = pri.GetPri();
		printf("%d ", ret);
	}
	printf("\n");
	return 0;
}

int
main(int argc, char *argv[])
{
	return test_pri();
}