#include "mem_file.h"
#include <IO.H>
#include <FCNTL.H>
#include <stdio.h>
#include <assert.h>
#include "mask_com.h"
#include <time.h>
#include "zlib.h"
#include "zlib_compress.h"


#define TEST_FILE	"d:/remote/mktdt00.txt"
//#define TEST_FILE	"mktdt00.txt"

page_file		pf_old;
page_file		pf_new;
page_file		pf_res;
MMaskCom		g_mask_com;


int
get_mask_com_size()
{
	unsigned int		i, ret = 0;
	

	for(i = 0; i < BLOCK_LEVELS; ++i){
		//printf("mask[%d][%d]\n", i, g_mask_com.m_data.level_data[i].size);
		ret += g_mask_com.m_data.level_data[i].size;
	}

	return ret;
}


int
get_mask_compress_size()
{
	compress_ctx		com_ctx;
	unsigned int		i, j, ret = 0;
	unsigned int		count, off;
	
	my_compress_init(&com_ctx, Z_DEFAULT_COMPRESSION);
	for(i = 0; i < BLOCK_LEVELS; ++i){
		count = g_mask_com.m_data.level_data[i].size / PAGE_SIZE;
		off = g_mask_com.m_data.level_data[i].size %PAGE_SIZE;
		for(j = 0; j < count; ++j){
			my_compress(&com_ctx, g_mask_com.m_data.level_data[i].page[j], PAGE_SIZE);
		}
		my_compress(&com_ctx, g_mask_com.m_data.level_data[i].page[j], off);
	}

	my_compress_final(&com_ctx);
	my_compress_destroy(&com_ctx);

	return com_ctx.amount;
}

int
get_data_compress_size(page_srv *ps)
{
	compress_ctx		com_ctx;
	unsigned int		i, count, off;

	count = ps->p_res->count / PAGE_SIZE;
	off = ps->p_res->count % PAGE_SIZE;

	my_compress_init(&com_ctx, Z_DEFAULT_COMPRESSION);
	for(i = 0; i < count; ++i){
		my_compress(&com_ctx, ps->p_res->page[i], PAGE_SIZE);
	}
	my_compress(&com_ctx, ps->p_res->page[i], off);

	my_compress_final(&com_ctx);
	my_compress_destroy(&com_ctx);
	
	return com_ctx.amount;
}

void
print_data(page_srv *ps)
{
	if(ps->p_old->count != ps->p_new->count){
		printf("文件大小由[%u]->[%u]\n", ps->p_old->count, ps->p_new->count);
		return ;
	}

	if(ps->p_res->count == 0){
		printf("文件没有发生变化\n");
		return;
	}

	printf("文件变化字节[%u:%u], 掩码[%u:%u]\n", ps->p_res->count, get_data_compress_size(ps),
									get_mask_com_size(), get_mask_compress_size());


}


int
test()
{
	int				err;
	int				fd;
	page_srv		ps;

	ps.p_old = &pf_old;
	ps.p_new = &pf_new;
	ps.p_res = &pf_res;
	fd = open(TEST_FILE, O_RDONLY);
	if(fd == -1){
		printf("打开文件失败[%d]\n", GetLastError());
		return -1;
	}
	g_mask_com.Init();
	page_file_init(ps.p_old);
	page_file_init(ps.p_new);
	page_file_init(ps.p_res);
	err = page_file_load(ps.p_old, fd);
	if(err)
		return err;
	close(fd);

	while(1){
		fd = open(TEST_FILE, O_RDONLY);
		if(fd == -1){
			printf("打开文件失败[%d]\n", GetLastError());
			return -1;
		}
		
		page_file_reinit(ps.p_new);
		page_file_reinit(ps.p_res);
		err = page_file_load(ps.p_new, fd);
		if(err)
			return err;
		close(fd);

		//ps.p_new->page[0][0] = (unsigned char)time(NULL);
		err = compare_pf(&ps);
		if(err)
			return err;

		print_data(&ps);

		swip_pf(&ps);

		Sleep(3000);
	}



	return err;
}

int
main(int argc, char *argv[])
{
	return test();
}