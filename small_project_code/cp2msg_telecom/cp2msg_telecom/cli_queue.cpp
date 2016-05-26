#include "cli_queue.h"
#include <assert.h>
#include "LocalLock.h"

MCliQue::MCliQue()
{
	::InitializeCriticalSection(&m_Section);
	_Init();
}

MCliQue::~MCliQue()
{

}

bool
MCliQue::Insert(int sock, char *ip, unsigned short port)
{
	MLocalSection			locSec;

	locSec.Attch(&m_Section);
	if(list_empty(&s_FreeHead))
		return false;

	tagCliListNode *pNode = list_entry(s_FreeHead.next, tagCliListNode, s_list_node);
	assert(pNode != NULL);

	pNode->cli_info.sock = sock;
	strncpy(pNode->cli_info.ip, ip, MAX_IP_LEN);
	pNode->cli_info.port = port;

	_Del2Insert(&(pNode->s_list_node), &s_UsedHead);
	return true;
}

bool
MCliQue::Del(int sock)
{
	MLocalSection			locSec;
	struct list_head		*ptmp, *psafe;		
	
	locSec.Attch(&m_Section);
	if(list_empty(&s_UsedHead))
		return false;

	list_for_each_safe(ptmp, psafe, &s_UsedHead){
		tagCliListNode *pNode = list_entry(ptmp, tagCliListNode, s_list_node); assert(pNode != NULL);
		if(pNode->cli_info.sock == sock){
			_Del2Insert(&(pNode->s_list_node), &s_FreeHead);
			return true;
		}
	}
	
	return false;
}

bool
MCliQue::GetRandSock(int &sock)
{
	MLocalSection			locSec;

	sock = -1;
	locSec.Attch(&m_Section);
	if(list_empty(&s_UsedHead))
		return false;

	tagCliListNode	*pNode = list_entry(s_UsedHead.next, tagCliListNode, s_list_node);
	assert(pNode != NULL); assert(pNode->cli_info.sock > 0);	
	sock = pNode->cli_info.sock;

	return true;
}



void
MCliQue::_Init()
{
	unsigned int				i;
	
	INIT_LIST_HEAD(&s_FreeHead);
	INIT_LIST_HEAD(&s_UsedHead);
	for(i = 0; i < CLI_LIST_LEN; ++i){
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
MCliQue::_Del2Insert(struct list_head *lhs, struct list_head *rhs)
{
	assert(lhs != NULL);
	assert(rhs != NULL);

	list_del(lhs);
	list_add_tail(lhs, rhs);
}


