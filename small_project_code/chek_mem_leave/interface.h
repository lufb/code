/*
	供外层调用的接口文件
*/

#ifndef _LUFUBO_MEM_CHEK_LEAVE_INTERFACE_H_
#define _LUFUBO_MEM_CHEK_LEAVE_INTERFACE_H_

#include "src/mem_info.h"


int mem_init(struct mem_record &mem,	/*	记录内存使用情况的对象	*/	
		 unsigned int interval,			/*	记录日志的时间间隔		*/
		 char *record_name				/*	记录文件名				*/
		 );

void
add_mem(struct mem_record &mem,		/*	记录内存使用情况的对象	*/
		char *file_name,			/*	申请内存的文件名		*/
		unsigned short line_no,		/*	申请内存的行号			*/
		unsigned int addr,			/*	内存起始地址			*/
		unsigned short size = 0		/*	申请内存的大小			*/
		);

void
del_mem(struct mem_record &mem,		/*	记录内存使用情况的对象	*/
		unsigned int addr			/*	删除内存的地址			*/
		);

#endif