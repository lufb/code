#include "zlib_compress.h"
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <memory.h>
#include <WINDOWS.H>

#define		IN_FILE		"./mktdt00.txt"
#define		COM_FILE	"./com.txt"
#define		OUT_FILE	"./restlt.txt"

#define		PER_SIZE	4096

FILE	*f_in,	*f_com, *f_res;
unsigned char	*in_buf, *com_buf, *res_buf;
unsigned int	in_file_size;





int
get_file_size(FILE *fp)
{
	int			f_size;
	
	fseek(fp, 0, SEEK_END);
	f_size =  ftell(fp);
	fseek(fp, 0, 0);
	
	return f_size;
}

int
test_init()
{
	int		rc;
	f_in	= fopen(IN_FILE, "rb");
	f_com	= fopen(COM_FILE, "wb");
	f_res	= fopen(OUT_FILE, "rb");

	if(	f_in == NULL ||
		f_com == NULL ||
		f_res == NULL){
		printf("打开文件出错\n");
		return -1;
	}

	in_file_size = get_file_size(f_in);
	in_buf = (unsigned char *)malloc(in_file_size);
	if((rc = fread(in_buf, 1, in_file_size, f_in)) != in_file_size){
		printf("fread error\n");
		return -2;
	}

	return 0;
}



void
test_destroy()
{
	fclose(f_in);
	fclose(f_com);
	fclose(f_res);
}


int 
main()
{
	int				rc;
	compress_ctx	ctx;
	unsigned int	i, count, leave;
	unsigned int	com_size;

	if((rc = test_init()) != 0)
		return rc;

	
	
	com_buf = (unsigned char *)malloc(1024*1024*100);
	res_buf = (unsigned char *)malloc(1024*1024*1024);
	assert(com_buf != NULL && res_buf != NULL);
	assert(com_buf != NULL);
	count = in_file_size/PER_SIZE;
	leave = in_file_size%PER_SIZE;

	if((rc = my_compress_init(&ctx, Z_BEST_COMPRESSION)) != 0)
		return rc;
	for(i = 0; i < count; ++i){
		my_compress(&ctx, in_buf+i*PER_SIZE, PER_SIZE, com_buf);
	}
	my_compress(&ctx, in_buf+count*PER_SIZE, leave, com_buf);

	my_compress_final(&ctx, com_buf);

	com_size = ctx.amount;

	fwrite(com_buf, 1, ctx.amount, f_com);

	my_compress_destroy(&ctx);

	printf("压缩[%u]->[%u]，所用时间\n", in_file_size, ctx.amount);

	//开始测试解压

	

	count = com_size/PER_SIZE;
	leave = com_size%PER_SIZE;

	my_decompress_init(&ctx);
	for(i = 0; i < count; ++i){
		my_decompress(&ctx, com_buf+i*PER_SIZE, PER_SIZE, res_buf);
	}
	my_decompress(&ctx, com_buf+i*PER_SIZE, leave, res_buf);
	printf("解压[%u]->[%u]，所用时间\n", com_size, ctx.amount);
	
	/*rc = fwrite(res_buf, 1, ctx.amount, f_res);
	if(rc != ctx.amount){
		printf("写结果文件有错[%d]\n", GetLastError());
		return -1;
	}*/
	if(memcmp(in_buf, res_buf, in_file_size)){
		printf("压缩算法有误\n");
	}


	return 0;


}