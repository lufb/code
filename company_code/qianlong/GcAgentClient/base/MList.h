/************************************************************************/
/* 文件名:                                                              */
/*			base/MList.h												*/
/* 功能:																*/
/*			封装一些list的基本操作										*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
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