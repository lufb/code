#include <stdio.h>
#include "error.h"
#include "mem_vf.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>


#define			SRC_FILE2		"./src2.txt"
#define			RST_FILE2		"./rst2.txt"

#define			SRC_FILE1		"./src1.txt"
#define			RST_FILE1		"./rst1.txt"	

unsigned int 
get_micsecond()
{
	return GetTickCount();
}

/*生成随机数[min, max)*/
int
creat_rand(int min, int max)
{
	//assert(min <= max);
	int					offset = max-min;
	if(offset == 0)
		return min;
	
	return rand()%offset + min;
}	

int test(MVF &vf, const char *src, const char *dst)
{
	unsigned char	buf[6000];
	int				once_read;
	int				err;
	int				fd_src = open(src, O_RDONLY|O_BINARY);
	int				fd_dst = open(dst, O_WRONLY|O_TRUNC|O_BINARY);
	int				total = 0;
	
	if(fd_src == -1){
		printf("打开文件1失败[%d]\n", GetLastError());
		return -1;
	}
	if(fd_dst == -1){
		printf("打开文件2失败[%d][%s]]\n", GetLastError(), dst);
		return -1;
	}
	vf.Open();
	while(1){
		once_read = creat_rand(1, sizeof(buf));
		//once_read = 1;
		err = read(fd_src, buf, once_read);	/*	lufubo 这里要调*/
		if(err == -1){
			printf("读文件出错[%d]\n", GetLastError());
			return -1;
		}
		if(err == 0)
			break;
		if(vf.Read(buf, err) < 0)
		{
			printf("error\n");
			return -1;
		}
	}
	vf.WriteAll(fd_dst);
	close(fd_src);
	close(fd_dst);
	return 0;

}

int main()
{
	MVF				vf;
	int					err;
	int					i;

	srand(time(NULL));

	
	if((err = test(vf, SRC_FILE2, RST_FILE2)) != 0)
		return err;
	if((err = test(vf, SRC_FILE1, RST_FILE1)) != 0)
 		return err;
	vf.Destroy();
	printf("test OK\n");
}
