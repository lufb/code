#include <stdio.h>
#include <assert.h>
#include "MList.h"
#include "error.h"


/**
 *	_del2add		-			将左操作数节点，从链表中删除，并加到右操作数的链表尾中
 *
 *	@lhs:			[in]		从哪个链表中去掉
 *	@lhs:			[in]		加到哪个链表中去
 *	
 *	Note:
 *								左操作数是个节点，右操作数是个链表头节点
 *															
 *	return
 *		==0						成功
 *		!=0						失败
 */
int	MList::_del2insert(struct list_head *lhs, struct list_head *rhs)
{
	assert(lhs != NULL);
	assert(rhs != NULL);

	list_del(lhs);
	list_add_tail(lhs, rhs);

	return 0;
}