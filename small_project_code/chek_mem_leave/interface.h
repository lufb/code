/*
	�������õĽӿ��ļ�
*/

#ifndef _LUFUBO_MEM_CHEK_LEAVE_INTERFACE_H_
#define _LUFUBO_MEM_CHEK_LEAVE_INTERFACE_H_

#include "src/mem_info.h"


int mem_init(struct mem_record &mem,	/*	��¼�ڴ�ʹ������Ķ���	*/	
		 unsigned int interval,			/*	��¼��־��ʱ����		*/
		 char *record_name				/*	��¼�ļ���				*/
		 );

void
add_mem(struct mem_record &mem,		/*	��¼�ڴ�ʹ������Ķ���	*/
		char *file_name,			/*	�����ڴ���ļ���		*/
		unsigned short line_no,		/*	�����ڴ���к�			*/
		unsigned int addr,			/*	�ڴ���ʼ��ַ			*/
		unsigned short size = 0		/*	�����ڴ�Ĵ�С			*/
		);

void
del_mem(struct mem_record &mem,		/*	��¼�ڴ�ʹ������Ķ���	*/
		unsigned int addr			/*	ɾ���ڴ�ĵ�ַ			*/
		);

#endif