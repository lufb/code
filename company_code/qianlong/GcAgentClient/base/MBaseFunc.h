/************************************************************************/
/* 文件名:                                                              */
/*			base/MBaseFunc.h											*/
/* 功能:																*/
/*			定义一些最基本的操作										*/
/* 特殊说明:															*/
/*			无															*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_BASE_FUNC_H_
#define _M_BASE_FUNC_H_

/** select的超时时间，单位(秒)*/
#define SELECT_TIME_OUT					1

/** 设置一个timeval的时间*/
#define INIT_TIMEVAL(_timeval_, _timeout_)		\
	do{											\
	_timeval_.tv_sec = _timeout_;				\
	_timeval_.tv_usec = 0;						\
	}while(0)


int									waitThreadExit(size_t count, HANDLE *handle, unsigned long millisecond );

#endif