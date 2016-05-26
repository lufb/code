#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int			i;
	struct stat		buf;
	char			*ptr;

	for(i = 1; i < argc; ++i){
		printf("%s: ", argv[i]);
		if(lstat(argv[1], &buf) < 0){
			printf("lstat %s error %s",
			argv[i], strerror(errno));
			
			continue;
		}

		if(S_ISREG(buf.st_mode))
			ptr = "regular";
		else if(S_ISDIR(buf.st_mode))
			ptr = "directory";
		else if(S_ISCHR(buf.st_mode))
			ptr = "character special";
		else if(S_ISBLK(buf.st_mode))
			ptr = "symbolic link";
		else if(S_ISBLK(buf.st_mode))
			ptr = "block file";
		else if(S_ISFIFO(buf.st_mode))
			ptr = "fifo";
		else if(S_ISSOCK(buf.st_mode))
			ptr = "sock";
		else
			ptr = "unknown mode **";	
		
		printf("%s\n", ptr);
	}

	return 0;
}

