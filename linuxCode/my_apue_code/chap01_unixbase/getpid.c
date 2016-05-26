#include <stdio.h>

int main(void)
{
	printf("this process pid: %d\n", getpid());
	printf("this process parent's id: %d\n", getppid());

	return 0;
}
