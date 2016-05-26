#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define		MAXLINE		1024

static void sig_int(int);

int main(void)
{
	char			buf[MAXLINE];
	pid_t			pid;
	int			status;

	if(signal(SIGINT, sig_int) == SIG_ERR){
		printf("signal error_msg: %s\n", strerror(errno));
		
		return -1;
	}

	printf("%% ");
	while(fgets(buf, MAXLINE, stdin) != NULL){
		if(buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;

		if((pid = fork()) < 0){
			perror("fork");
			return -2;
		}else if(pid == 0){		/* child */
			execlp(buf, buf, (char *)0);
			/* only failed can exec low code*/
			/* because if succed this code will be replaced by new image */
			printf("conldn't execute: %s\n", buf);
			exit(127);
		}
		
		/* parent */
		if((pid = waitpid(pid, &status, 0)) < 0)
			perror("waitpid");

		printf("pid: %d exit with code: %d\n", pid, status);
		
		printf("%% ");
	}
	
	return 0;
}

void sig_int(int signo)
{
	printf("interrupt\n%% ");
}
