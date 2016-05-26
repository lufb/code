#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int main(void)
{
	if(lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
		printf("STDIN_FILENO can't lseek: %s\n", strerror(errno));
	else
		printf("STDIN_FILENO can lseek\n");

	return 0;
}
