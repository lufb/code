#include <stdio.h>
#include <errno.h>

int main(void)
{
	int			fd;

	printf("begin errno = %d\n", errno);
	
	errno = 12;
	perror("malloc");

	/* need reinit, otherwise the value of errno is indeterminacy */
	errno = 12;
	printf("errno = %d, errinfo: %s", errno, strerror(errno));

	return 0;
}
