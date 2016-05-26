#include "mem_file.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "mask_com.h"

extern MMaskCom		g_mask_com;

void
page_file_init(page_file *p_f)
{
	assert(p_f != NULL);
	memset(p_f, 0, sizeof(page_file));
}

void
page_file_reinit(page_file *p_f)
{
	p_f->count = 0;
}

int
_my_malloc(page_file *p_f)
{
	if(p_f->used_page == MAX_PAGE)
		return -2;		/*	文件超过最大值	*/
	p_f->page[p_f->used_page] = (unsigned char*)malloc(PAGE_SIZE);
	if(NULL == p_f->page[p_f->used_page])
		return -3;		/*	申请内存失败	*/
	
	++(p_f->used_page);
	return 0;
}

void
swip_pf(page_srv *ps)
{
	page_file *tmp = ps->p_new;
	ps->p_new = ps->p_old;
	ps->p_old = tmp;
}

int
add_mask(unsigned int offset)
{
	return g_mask_com.Compress(offset, BLOCK_LEVELS-1);
}

int
add_data(page_srv *ps, unsigned char data)
{
	int					rc;
	unsigned int		page, P_off;
	
	page = ps->p_res->count % MAX_PAGE;
	P_off = ps->p_res->count / MAX_PAGE;

	if(page >= ps->p_res->used_page){
		rc = _my_malloc(ps->p_res);
		if(rc)
			return rc;
	}

	ps->p_res->page[page][P_off] = data;
	++(ps->p_res->count);

	return 0;
}


int
compare_int(page_srv *ps, unsigned int offset, unsigned int *p_int_new, unsigned int *p_int_old)
{
	int				rc;
	unsigned int	i;
	unsigned char	*p_c_new = (unsigned char *)p_int_new;
	unsigned char	*p_c_old = (unsigned char *)p_int_old;
	
	
	for(i = 0; i < 4; ++i){
		if(p_c_new[i] != p_c_old[i]){
			if(rc = add_mask(offset + i))
				return rc;
			if(rc = add_data(ps, p_c_new[i]))
				return rc;
		}
	}
	
	return 0;
}


int
compare_pf(page_srv *ps)
{
	if(ps->p_new->count != ps->p_old->count){
		return 0;
	}
	int					err;
	unsigned int		i, j;
	unsigned int		*p_int_new, *p_int_old;
	unsigned int		offset = 0;
	unsigned int		pages = ps->p_new->count / PAGE_SIZE;
	unsigned int		P_off = ps->p_new->count % PAGE_SIZE;
	
	for(i = 0; i < pages; ++i){
		p_int_new	= (unsigned int *)(ps->p_new->page[i]);
		p_int_old	= (unsigned int *)(ps->p_old->page[i]);
		for(j = 0; j < PAGE_4; ++j){
			if(p_int_new[j] != p_int_old[j]){
				if(err = compare_int(ps, offset + j * 4,  p_int_new + j, p_int_old + j))
					return err;
			}
		}
	}
	
	offset = i * PAGE_SIZE;
	//	开始处理不足一页的数据
	p_int_new = (unsigned int *)(ps->p_new->page[pages]);
	p_int_old = (unsigned int *)(ps->p_old->page[pages]);
	for(j = 0; j < P_off/4; ++j){
		if(p_int_new[j] != p_int_old[j]){
			if(err = compare_int(ps, offset + j * 4, p_int_new + j, p_int_old + j))
				return err;
		}
	}
	
	return 0;
}

int
page_file_load(page_file *p_f, int fd)
{
	assert(p_f != NULL);

	int				err;
	unsigned int	next_page = 0;
	unsigned int	leave, pages, P_offset;
	
	while(1){
		assert(next_page <= p_f->used_page);
		if(next_page >= p_f->used_page){
			err = _my_malloc(p_f);
			if(err)		
				return err;
		}
		
		err = read(fd, p_f->page[next_page++], PAGE_SIZE);	/*	lufubo 这里要调*/
		if(err == -1){
			printf("读文件出错[%d]\n", GetLastError());
			return -1;
		}
		//printf("err[%d][%d]\n", err, p_f->used_page);
		p_f->count += err;
		if(err == 0)
			break;
	}
	leave = p_f->count % 4;
	if(leave){	/*	需要将不足4个字节的字节后面补全0	*/
		pages		= p_f->count / PAGE_SIZE;
		P_offset	= p_f->count % PAGE_SIZE;
		memset((void*)(p_f->page[pages] + P_offset), 0, leave);
	}

	return 0;
}