/************************************************************************/
/* 文件名:                                                              */
/*			base/MNetRand.h												*/
/* 功能:																*/
/*			定义得到一个在网络上相对不重复的随机数						*/
/* 特殊说明:															*/
/*			无															*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-12-04	卢富波		创建								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_NET_RAND_H_
#define _M_NET_RAND_H_

#include <httpext.h>  

class MNetRand
{
public:
							MNetRand();
							~MNetRand();
	
	unsigned long			getARand();
	
private:
	void					getMac();
	void					getMem();
	
private:
	char					m_cMacInfo[256];
	MEMORYSTATUS			m_cMemInfo;
};



#endif