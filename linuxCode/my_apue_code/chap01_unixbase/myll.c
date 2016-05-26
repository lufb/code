#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
	DIR			*dp;
	struct dirent		*dirp;

	if(argc != 2){
		printf("usage: ls directory_name\n");
		return -1;
	}

	if((dp = opendir(argv[1])) == NULL){
		perror("opendir");
		return -2;
	}

	while((dirp = readdir(dp)) != NULL)
		printf("%s\n", dirp->d_name);

	closedir(dp);
	
	return 0;
}
