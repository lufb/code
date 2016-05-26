#include <stdio.h>
#include <fcntl.h>
#include <utime.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int			i, fd;
	struct stat		statbuf;
	struct utimbuf		timebuf;

	for(i = 1; i < argc; ++i){
		if(stat(argv[i], &statbuf) < 0){
			printf("stat %s error: %s\n",
				argv[i], strerror(errno));
			
			continue;			
		}
		
		if((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0){
			printf("trunk %s error: %s\n", argv[i], strerror(errno));
			
			continue;
		}


		close(fd);
		timebuf.actime = statbuf.st_atime;
		timebuf.modtime = statbuf.st_mtime;
		
		if(utime(argv[i], &timebuf) < 0){
			printf("utime %s error: %s\n", argv[i], strerror(errno));
	
			continue;
		}
	}

	return 0;
}
