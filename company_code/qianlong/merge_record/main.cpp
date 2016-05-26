#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "cmd.h"
#include "error.h"

char		*ptmpsrc = NULL;
char		*ptmpdst = NULL;

void
cleanup(char *file)
{
	char cmd[256] = {0};
	
	sprintf(cmd, "rm -f %s", file);
	system(cmd);
}

void 
my_exit(void)
{
	cleanup(ptmpsrc);
	cleanup(ptmpdst);
}


int 
main(int argc, char *argv[])
{
	int			err;
	
	if((err =chekparam(argc, argv)) != 0){
		pr_usage();
		return err;
	}
	
 	ptmpsrc = (char *)malloc(256);
 	if(ptmpsrc == NULL){
 		return BUILD_ERROR(0, E_FMEM);
 	}
	ptmpdst = (char *)malloc(256);
	if(ptmpdst == NULL){
		return BUILD_ERROR(0, E_FMEM);
 	}

 	sprintf(ptmpsrc, "%s_%d", "/tmp/ql_lufb_tmp_src", getpid());
	sprintf(ptmpdst, "%s_%d", "/tmp/ql_lufb_tmp_dst", getpid());
	cleanup(ptmpsrc);
	cleanup(ptmpdst);
	if(atexit(my_exit) != 0){      /*  */
        printf("atexit my_exit1 error\n");		
        return BUILD_ERROR(0, E_REG);
    }

	if((err = join_file(argc, argv, ptmpsrc, ptmpdst)) != 0){
		my_exit();
		return err;
	}

	/*begin to handle each user's record */
	err = deal_records(argv[2], ptmpdst);
	if(err == 0)
		printf("sucess, the result is in [%s] \n", argv[2]);
	else
		printf("failed with errCode[%d(%d:%d)]\n", err, GET_SYS_ERR(err), GET_USER_ERR(err));

	my_exit();

	return err;
}

