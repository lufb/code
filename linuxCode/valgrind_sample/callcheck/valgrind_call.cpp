#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

void
test()
{
	sleep(1);
}

void
f()
{
	int i;
	for(i = 0; i < 5; ++i)
		test();
}

int
main(void)
{
	f();
	printf("is over\n");
	return 0;
}
