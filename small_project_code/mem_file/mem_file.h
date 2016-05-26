#ifndef _MEM_FILE_H_
#define _MEM_FILE_H_

#ifndef LINUXCODE
#include <wtypes.h>
#else
#endif


#define PAGE_SIZE	4096							/*	一个页面的大小				*/
#define MAX_PAGE	4096000/PAGE_SIZE	/*	1G文件(262141)				*/
#define	PAGE_4		PAGE_SIZE/4					/*	按4字节循环一个页面时的次数	*/

typedef struct _page_file{
	unsigned int		count;					/*	计数						*/
	unsigned char		*page[MAX_PAGE];		/*	指向页面					*/
	unsigned int		used_page;				/*	已申请页面计数				*/
}page_file;

typedef struct _page_srv{
	page_file			*p_old;					/*	老数据						*/
	page_file			*p_new;					/*	新数据						*/
	page_file			*p_res;					/*	压缩的变动数据				*/
}page_srv;


void					page_file_init(page_file *p_f);
void					page_file_reinit(page_file *p_f);

int						page_file_load(page_file *p_f, int fd);

int						compare_pf(page_srv *ps);
void					swip_pf(page_srv *ps);



#endif

