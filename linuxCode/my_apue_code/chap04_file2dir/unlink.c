#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if(open("tempfile", O_RDWR) < 0){
		perror("open");

		return -1;
	}

	if(unlink("tempfile") < 0){
		perror("unlink");
		
		return -2;
	}

	printf("file: %s unlinked\n");
	
	sleep(15);
	printf("done\n");
	
	return 0;
}
