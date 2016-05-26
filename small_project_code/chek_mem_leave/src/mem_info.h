#ifndef _LUFUBO_MEM_INFO_H_
#define _LUFUBO_MEM_INFO_H_

#include <wtypes.h>
#include <stdio.h>
#include <assert.h>
#include "../lib/list.h"

#define MAX_RECORD		4096					/*	��¼�ڴ����������¼��	*/
#define MAX_FILE_NAME	128						/*	��¼�ļ�����󳤶ȣ������ó����ļ�����ض�	*/


/*	����������ڴ�ṹ	*/
struct mem_unit{
	unsigned int		addr;					/*	�ڴ����ʼ��ַ				*/
	unsigned short		addr_size;				/*	�����ڴ�Ĵ�С(�ֽ�)		*/
	unsigned short		file_no;				/*	�����ڴ���к�				*/
	char				file_name[MAX_FILE_NAME];/*	�����ڴ���ļ���			*/
};

/*	����������ڴ�ṹ������ṹ	*/
struct mem_node{
	struct mem_unit		one_mem;				/*	������ڴ���Ϣ	*/
	struct list_head	list_node;				/*	����ڵ�		*/
	
	void init(){
		memset(&one_mem, 0, sizeof(struct mem_node));
	}
};

/*	��¼�������ڴ�ļ�¼��Ϣ	*/
struct mem_record{
	CRITICAL_SECTION	lock;					/*	���ڵ�,��ʱֻ����windows		*/
	unsigned int		scan_interval;			/*	��ӡ�ڴ�ʹ�ü�¼�ļ��ʱ��(s)	*/
	struct mem_node		mem_info[MAX_RECORD];	/*	�ڴ���Ϣ����					*/
	struct list_head	used;					/*	�����������ڴ�����				*/
	struct list_head	free;					/*	���ӿ�������					*/
	FILE				*fp;					/*	д��¼���ļ�					*/
};

#endif