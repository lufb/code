#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

char	buf1[] = "abcdefghij";
char	buf2[] = "ABCDEFGHIJ";

int main(int argc, char *argv)
{
	int		fd;
	
	if((fd = open("file.hole", O_WRONLY | O_CREAT | O_TRUNC, FILE_MODE)) < 0){
		printf("open error: %s\n", strerror(errno));
		
		return -1;
	}

	if(write(fd, buf1, 10) != 10){
		perror("write1");
		
		return -2;
	}

	if(lseek(fd, 16384, SEEK_SET) == -1){
		perror("lseek");
		
		return -3;
	}

	if(write(fd, buf2, 10) != 10){
		perror("write2");
		
		return -4;
	}
	
	printf("create hole file sucess\n");
	
	return 0;
}
