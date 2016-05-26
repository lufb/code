#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int	fileexist(char *filename);

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("usage: a.out <testfilename>");
		
		return -1;
	}
	
	return fileexist(argv[1]);
}

int fileexist(char *filename)
{
	if((open(filename, O_CREAT | O_EXCL)) == -1){
		printf("file: %s is existed\n", filename);
		
		return 1;
	}

	printf("file: %s is not existed\n", filename);
	
	return 0;
}
	
