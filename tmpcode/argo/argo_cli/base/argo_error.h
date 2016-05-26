/************************************************************************/
/* 文件名:                                                              */
/*			base/argo_error.h											*/
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
#ifndef _ARGO_ERROR_H_
#define _ARGO_ERROR_H_

#ifdef WIN32
#include <windows.h>
#endif



/*	公共部份错误码	*/
#define		E_SUCESS		0			/* 正常，没出错		*/
#define		E_OS			1			/*	操作系统出错	*/	
#define		E_ABORT1		2			/* 断言出错			*/			
#define		E_LOGIC			3			/* 程序逻辑出错		*/
#define		E_MALLOC		4			/* 申请内存失败		*/
#define		E_RCVTIMEOUT	5			/* 收数据超时		*/
#define		E_SOCKCLOSE		6			/* 套接字关闭		*/
#define		E_DNS			7			/* DNS转换出错		*/
#define		E_MAX_BDE_SIZE	8			/* 一个BDE的大小越界*/
#define		E_REALLOC		9			/* realloc失败		*/
#define		E_UNBDETYPE		10			/* BDE类型出错		*/
#define		E_UNBDETYPE1	11			/* BDE类型有空包	*/
#define		E_UNBDETYPE2	12			/* BDE类型不能为空	*/
#define		E_MINHEAD_SIZE	13			/* 协议长度还不够最小长度*/
#define		E_UN_PROTYPE	14			/* 未知协议号		*/
#define		E_PRO_DIR		15			/* 收到的协议号方向错	*/
#define		E_OPTION_00		16			/* 服务器没有相关配置*/
#define		E_UN_FILETYPE	17			/* 不知道的文件类型	*/
#define		E_READ_FILE		18			/* 读文件发生错误	*/
#define		E_CRC			19			/* CRC校对出错		*/
#define		E_FILE_SIZE		20			/* 协议头大小与文件大小不一致*/
#define		E_FILE_HASH		20			/* 协议头HASH与文件HASH不一致*/
#define		E_CRT_DIR		21			/* 创建目录失败		*/
#define		E_WRITE_WRITE	22			/* 打开文件写出错	*/
#define		E_FILE_TIME		23			/* 更改文件时间出错	*/
#define		E_DEL_FILE		24			/* 删除文件失败		*/
#define		E_RENAME_FILE	25			/* 删除文件失败		*/
#define		E_OPEN_WRITE	26			
#define		E_CONNECT		27			/* connect失败		*/
#define		E_UP_HASH		28			/* 更新后hash不成功	*/
#define		E_LOAD_OP		29			/* 加载配置文件失败	*/
#define		E_MASK_NO_ZERO	30			/* 解码后mask数据不全为0*/
#define		E_UN_COMTYPE	31			/* 未知类型的压缩类型*/
#define		E_TIMEOUT_HEART	32			/* 过长时间没有收到服务器的数据	*/
#define		E_CR_THREAD		33			/* 创建工作线程失败	*/
#define		E_SET_NONBLOCK	34			/* 设置非阻塞套接字失败*/
#define		E_UNCOMPRESS	35			/* 解压数据出错		*/
#define		E_FISTFILE		36			/* 打开文件目录出错	*/
#define		E_RCV			37			/* 收数据出错		*/

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