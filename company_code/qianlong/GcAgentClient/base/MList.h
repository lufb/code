/************************************************************************/
/* �ļ���:                                                              */
/*			base/MList.h												*/
/* ����:																*/
/*			��װһЩlist�Ļ�������										*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-25	¬����		����								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_LIST_H_
#define _M_LIST_H_

#include "list.h"



class MList
{
public:
								MList();
								~MList();

	static int					_del2insert(struct list_head *lhs, struct list_head *rhs);
};


#endif