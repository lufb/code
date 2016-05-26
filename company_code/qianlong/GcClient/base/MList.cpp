#include <stdio.h>
#include <assert.h>
#include "MList.h"
#include "error.h"


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
 *		==0						�ɹ�
 *		!=0						ʧ��
 */
int	MList::_del2insert(struct list_head *lhs, struct list_head *rhs)
{
	assert(lhs != NULL);
	assert(rhs != NULL);

	list_del(lhs);
	list_add_tail(lhs, rhs);

	return 0;
}