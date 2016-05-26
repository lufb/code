#ifndef _MEM_FILE_H_
#define _MEM_FILE_H_

#ifndef LINUXCODE
#include <wtypes.h>
#else
#endif


#define PAGE_SIZE	4096							/*	һ��ҳ��Ĵ�С				*/
#define MAX_PAGE	4096000/PAGE_SIZE	/*	1G�ļ�(262141)				*/
#define	PAGE_4		PAGE_SIZE/4					/*	��4�ֽ�ѭ��һ��ҳ��ʱ�Ĵ���	*/

typedef struct _page_file{
	unsigned int		count;					/*	����						*/
	unsigned char		*page[MAX_PAGE];		/*	ָ��ҳ��					*/
	unsigned int		used_page;				/*	������ҳ�����				*/
}page_file;

typedef struct _page_srv{
	page_file			*p_old;					/*	������						*/
	page_file			*p_new;					/*	������						*/
	page_file			*p_res;					/*	ѹ���ı䶯����				*/
}page_srv;


void					page_file_init(page_file *p_f);
void					page_file_reinit(page_file *p_f);

int						page_file_load(page_file *p_f, int fd);

int						compare_pf(page_srv *ps);
void					swip_pf(page_srv *ps);



#endif

