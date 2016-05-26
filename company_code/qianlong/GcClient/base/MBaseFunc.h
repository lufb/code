/************************************************************************/
/* �ļ���:                                                              */
/*			base/MBaseFunc.h											*/
/* ����:																*/
/*			����һЩ������Ĳ���										*/
/* ����˵��:															*/
/*			��															*/
/* ��Ȩ:																*/
/*			CopyRight @¬���� <1164830775@qq.com>						*/
/* �޸���ʷ��															*/
/*			2013-11-25	¬����		����								*/
/*			                                                            */
/************************************************************************/

#ifndef _M_BASE_FUNC_H_
#define _M_BASE_FUNC_H_

/** select�ĳ�ʱʱ�䣬��λ(��)*/
#define SELECT_TIME_OUT					1

/** ����һ��timeval��ʱ��*/
#define INIT_TIMEVAL(_timeval_, _timeout_)		\
	do{											\
	_timeval_.tv_sec = _timeout_;				\
	_timeval_.tv_usec = 0;						\
	}while(0)


int									waitThreadExit(size_t count, HANDLE *handle, unsigned long millisecond );

#endif