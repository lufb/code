/************************************************************************/
/* 文件名:                                                              */
/*			base/error.h												*/
/* 功能:																*/
/*			声明错误类型及获取相应错误的宏								*/
/* 特殊说明:															*/
/*			无															*/
/* 版权:																*/
/*			CopyRight @卢富波 <1164830775@qq.com>						*/
/* 修改历史：															*/
/*			2013-11-25	卢富波		创建								*/
/*			                                                            */
/************************************************************************/
#ifndef _ERROR_H_
#define _ERROR_H_

#ifdef WIN32
#include <windows.h>
#endif



/*	公共部份错误码	*/
#define		E_SUCESS		0			/* 正常，没出错		*/
#define		E_ABORT1		2			/* 断言出错			*/			
#define		E_LOGIC			3			/* 程序逻辑出错		*/
#define		E_MALLOC		4			/* 申请内存失败		*/


/*	虚拟文件错误码	*/
#define		VF_BASE			1000		/* 虚拟文件错误码基数*/
#define		VF_BIG_SIZE		VF_BASE+1	/* 超过虚拟文件最大支持字节数	*/
#define		VF_WRT_DISK		VF_BASE+2	/* 写磁盘文件出错	*/
#define		VF_BEY_MAX		VF_BASE+3	/* 更新数据越界		*/

#ifdef WIN32
#define	_OSerrno()			GetLastError()
#else
#include <errno.h>
#define	_OSerrno()			errno
#endif

/* 组装错误码:高16位为用户错误码，低16位为系统错误码 */
/* 0x80008000是为了保证组装的错误码必定是个负数 */
/* 要获相应错误码方法：用相应16位与7FFF相'&'就得到相应错误码 */
#define BUILD_ERROR(_sys_, _sp_) \
	(((_sys_) & 0xFFFF | (((_sp_) << 16) & 0xFFFF0000)) | 0x80008000)

/* 用组装的错误码得到操作系统的错误码 */
#define GET_SYS_ERR(_errCode_)		\
	((_errCode_) & 0x7FFF)

/* 用组装的错误码得到用户的错误码 */
#define GET_USER_ERR(_errCode_)		\
	(((_errCode_) & 0x7FFF0000) >> 16)




#endif