#ifndef _LUFUBO_MEM_INFO_H_
#define _LUFUBO_MEM_INFO_H_

#include <wtypes.h>
#include <stdio.h>
#include <assert.h>
#include "../lib/list.h"

#define MAX_RECORD		4096					/*	记录内存情况的最大记录数	*/
#define MAX_FILE_NAME	128						/*	记录文件的最大长度，超过该长度文件名会截断	*/


/*	描述申请的内存结构	*/
struct mem_unit{
	unsigned int		addr;					/*	内存的起始地址				*/
	unsigned short		addr_size;				/*	申请内存的大小(字节)		*/
	unsigned short		file_no;				/*	申请内存的行号				*/
	char				file_name[MAX_FILE_NAME];/*	申请内存的文件名			*/
};

/*	描述申请的内存结构的链表结构	*/
struct mem_node{
	struct mem_unit		one_mem;				/*	申请的内存信息	*/
	struct list_head	list_node;				/*	链表节点		*/
	
	void init(){
		memset(&one_mem, 0, sizeof(struct mem_node));
	}
};

/*	记录已申请内存的记录信息	*/
struct mem_record{
	CRITICAL_SECTION	lock;					/*	锁节点,暂时只考虑windows		*/
	unsigned int		scan_interval;			/*	打印内存使用记录的间隔时间(s)	*/
	struct mem_node		mem_info[MAX_RECORD];	/*	内存信息集合					*/
	struct list_head	used;					/*	串接已申请内存链表				*/
	struct list_head	free;					/*	串接空闲链表					*/
	FILE				*fp;					/*	写记录的文件					*/
};

#endif