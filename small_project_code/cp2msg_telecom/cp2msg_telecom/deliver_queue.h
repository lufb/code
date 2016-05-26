#ifndef _DELIVER_QUEUE_H_
#define _DELIVER_QUEUE_H_

#include "list.h"
#include "string.h"
#include "local_define.h"
#include <windows.h>

#define MAX_DELIVER_LEN		64			/* 存储DELIVER消息的长度	*/


typedef struct  {
	int				sock;				/* 往哪个套接字回			*/
	unsigned char	IsReport;			
	char			MsgID[10];			/* 如果是状态报告时，这个字段有用 */
} tagDeliverMainData;

typedef struct {
	tagDeliverMainData	del_info;
	struct list_head	s_list_node;
	
	void _init(){
		memset(&del_info, 0, sizeof(del_info));
	}
} tagDeliverMainDataNode;

class MDeliverQue{
public:
							MDeliverQue();
							~MDeliverQue();
	bool					Insert(int isReport, const char *msgid, int sock);
	bool					Del(tagDeliverMainData &deliver_data);
private:
	void					_Init();
	void					_Del2Insert(struct list_head *lhs, struct list_head *rhs);
private:
	struct list_head		s_UsedHead;
	struct list_head		s_FreeHead;
	tagDeliverMainDataNode	s_Data[MAX_DELIVER_LEN];
	CRITICAL_SECTION		m_Section;
};

#endif
