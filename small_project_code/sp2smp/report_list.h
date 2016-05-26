#ifndef _REPORT_H_H_
#define _REPORT_H_H_

#include <windows.h>
#include "list.h"
#include "interface.h"
/*
	有关report链表的操作
*/

#define	REPORT_LIST_SIZE			128		/*链表节点的个数*/

struct report_node{
	struct msg_report			rep;	/*网络字节序*/
	struct list_head			list_node;	/*本机字节序*/
};

struct report_list{
	report_list(){
		INIT_LIST_HEAD(&msg_used);
		INIT_LIST_HEAD(&msg_free);
		InitializeCriticalSection(&msg_lock);
		for(int i = 0; i < REPORT_LIST_SIZE; ++i){
			list_add_tail(&(rep_node_data[i].list_node), &msg_free);
		}
	}
	CRITICAL_SECTION			msg_lock;
	struct list_head			msg_used;
	struct list_head			msg_free;
	struct report_node			rep_node_data[REPORT_LIST_SIZE];
};

bool	had_message(struct report_list &);
int		del_report(struct msg_report &node, struct report_list &rep_list);
int		add_report(struct msg_report &rep_msg, struct report_list &rep_list);



#endif