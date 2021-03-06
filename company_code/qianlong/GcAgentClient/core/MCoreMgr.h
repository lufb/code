/************************************************************************/
/* 文件名:                                                              */
/*			core/MCoreMgr.h												*/
/* 功能:																*/
/*			GcClient主要处理流程的第0层模块	即入口						*/
/* 特殊说明:															*/
/*			GcClient主要处理流程分为七层								*/
/*			(0)MCoreMgr 模块	主要流程入口及管理模块					*/
/*			(1)MListen	模块	负责监听								*/
/*			(2)MRcvOpen	模块	负责收用户的协议信息,根据协议填充数据	*/
/*			(3)MCONN	模块	负责异步连接GcS(或者代理服务器)			*/
/*			(4)MAGENT	模块	负责代理的认证(如果用户通过代理连接)	*/
/*			(5)MHAND	模块	负责与GcS握手建立						*/
/*			(6)MTrans	模块	负责数据转发							*/
/*			其中下层模块仅供直接上层模块调用							*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/
#ifndef _M_CORE_MGR_H_
#define _M_CORE_MGR_H_

#include "MListen.h"


class MCoreMgr
{
public:
									MCoreMgr();
									~MCoreMgr();

	int								init(unsigned short port);
	void							destroy();
	void							waitExit(int *err, size_t size);
									
private:
	MListen							m_clLisMode;	/** listen module*/											
};

#endif