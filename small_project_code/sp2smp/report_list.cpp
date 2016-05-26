#include "report_list.h"
#include "LocalLock.h"
#include "error_define.h"
#include <assert.h>

bool 
had_message(struct report_list &rep_list)
{
	MLocalSection			locSec;
	
	locSec.Attch(&rep_list.msg_lock);
	if(list_empty(&rep_list.msg_used))
		return false;
	
	return true;
}

/**
 *	_del2add		-			����������ڵ㣬��������ɾ�������ӵ��Ҳ�����������β��
 *
 *	@lhs:			[in]		���ĸ�������ȥ��
 *	@lhs:			[in]		�ӵ��ĸ�������ȥ
 *	
 *	Note:
 *								��������Ǹ��ڵ㣬�Ҳ������Ǹ�����ͷ�ڵ�
 *															
 *	return
 *					��
 */
void	_del2insert(struct list_head *lhs, struct list_head *rhs)
{
	list_del(lhs);
	list_add_tail(lhs, rhs);
}

int
del_report(struct msg_report &node, struct report_list &rep_list)
{
	MLocalSection			locSec;
	
	if(list_empty(&rep_list.msg_used))
		return BUILD_ERROR(0, E_LIST_EMPTY);
	
	locSec.Attch(&rep_list.msg_lock);
	struct report_node *pNode = list_entry(rep_list.msg_used.next, struct report_node, list_node);
	assert(pNode != NULL);
	memmove(&node, &(pNode->rep), sizeof(node));
	_del2insert(&(pNode->list_node), &rep_list.msg_free);
	
	return 0;
}

int
add_report(struct msg_report &rep_msg, struct report_list &rep_list)
{
	MLocalSection			locSec;
	
	locSec.Attch(&rep_list.msg_lock);
	if(list_empty(&rep_list.msg_free))	//������
		return BUILD_ERROR(0, E_MSG_LEN);
	
	struct report_node	*pNode = list_entry(rep_list.msg_free.next, struct report_node, list_node);
	assert(pNode != NULL);
	memcpy(&(pNode->rep), &rep_msg, sizeof(struct msg_report));
	
	_del2insert(&(pNode->list_node), &rep_list.msg_used);


	return 0;
}