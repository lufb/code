#include "deliver_queue.h"
#include <assert.h>
#include "LocalLock.h"

MDeliverQue::MDeliverQue()
{
	::InitializeCriticalSection(&m_Section);
	_Init();
}

MDeliverQue::~MDeliverQue()
{

}

bool
MDeliverQue::Insert(int isReport, const char *msgid, int sock)
{
	MLocalSection			locSec;

	locSec.Attch(&m_Section);
	if(list_empty(&s_FreeHead))
		return false;

	tagDeliverMainDataNode *pNode = list_entry(s_FreeHead.next, tagDeliverMainDataNode, s_list_node);
	assert(pNode != NULL);

	pNode->del_info.sock = sock;
	pNode->del_info.IsReport = isReport;
	if(msgid != NULL)
		strncpy(pNode->del_info.MsgID, msgid, sizeof(pNode->del_info.MsgID));

	_Del2Insert(&(pNode->s_list_node), &s_UsedHead);

	return true;
}

bool
MDeliverQue::Del(tagDeliverMainData &deliver_data)
{
	MLocalSection			locSec;
	
	locSec.Attch(&m_Section);
	if(list_empty(&s_UsedHead))
		return false;

	tagDeliverMainDataNode			*pNode = list_entry(s_UsedHead.next, tagDeliverMainDataNode, s_list_node);
	assert(pNode != NULL);
	memcpy(&deliver_data, &(pNode->del_info), sizeof(deliver_data));
	
	_Del2Insert(&(pNode->s_list_node), &s_FreeHead);
	
	return true;
}

void
MDeliverQue::_Init()
{
	unsigned int				i;
	
	INIT_LIST_HEAD(&s_FreeHead);
	INIT_LIST_HEAD(&s_UsedHead);
	for(i = 0; i < MAX_DELIVER_LEN; ++i){
		s_Data[i]._init();
		list_add_tail(&s_Data[i].s_list_node, &s_FreeHead);
	}
}

/**
 *	_del2add		-			将左操作数节点，从链表中删除，并加到右操作数的链表尾中
 *
 *	@lhs:			[in]		从哪个链表中去掉
 *	@lhs:			[in]		加到哪个链表中去
 *	
 */
void	
MDeliverQue::_Del2Insert(struct list_head *lhs, struct list_head *rhs)
{
	assert(lhs != NULL);
	assert(rhs != NULL);

	list_del(lhs);
	list_add_tail(lhs, rhs);
}


