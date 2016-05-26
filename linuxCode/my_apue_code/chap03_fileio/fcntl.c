#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

void set_fl(int fd, int flags);

int main(int argc, char *argv[])
{
	int			val;
	
	if(argc != 2){
		printf("usage: a.out <descriptor#>");
		
		return -1;
	}

	/* open O_SYNC */
	set_fl(atoi(argv[1]), O_SYNC);

	if((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0 ){
		printf("fcntl with descriptor %d error: %s\n",
			atoi(argv[1]), strerror(errno));

		return -2;
	}	

	switch( val & O_ACCMODE ){
	case O_RDONLY:
		printf("read only");
		break;
	
	case O_WRONLY:
		printf("write only");
		break;

	case O_RDWR:
		printf("read and write");
		break;

	default:
		printf("unknown access mode");
		
		return -2;
	}
	
	if( val & O_APPEND )
		printf(", append");
	
	if( val & O_NONBLOCK )
		printf(", nonblocking");

#if defined( O_SYNC )
	if( val & O_SYNC )
		printf(", synchronous writes");
#endif

#if !defined( _POSIX_C_SOURCE ) && defined( O_FSYNC )
	if( val & O_FSYNC )
		printf(", synchronous writes");
#endif

	putchar('\n');

	return 0;
}


/* append another func */
void set_fl(int fd, int flags)
{
	int			val;
	
	if((val = fcntl(fd, F_GETFL, 0)) < 0){
		perror("fcntl");
		
		return;
	}

	val |= flags;

	if(fcntl(fd, F_SETFL, val) < 0){
		perror("fcntl2");
	}
}
