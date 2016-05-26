#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("usage: a.out <pathname>\n");
		
		return -1;
	}

	if(access(argv[1], R_OK) < 0){
		perror("access1");
		
		return -2;
	}else{
		printf("%s read access OK\n", argv[1]);
	}

	return 0;
}
