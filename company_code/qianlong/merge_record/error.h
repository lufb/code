#ifndef _ERROR_H_
#define _ERROR_H_

#define		E_OK			0			/* 正常，没出错 */
#define		E_TIME			1			/* 结尾时间小于了开始时间 */
#define		E_ASSERT		2			/* 程序断言错误 */
#define		E_USER			3			/* 用户传参错误 */
#define		E_EXIT			4			/* 用户的结果文件已经存在 */
#define		E_NOEXIT		5			/* 用户传入的源文件不存在 */
#define		E_BLANK			6			/* 为文件增加空行出错 */
#define		E_MERGE			7			/* 合并文件出错 */
#define		E_REMOVE		8			/* 删除文件中的空行出错 */
#define		E_SRC			9			/* 打开源文件进行读出错*/
#define		E_DST			10			/* 打开目标文件进行写出错*/
#define		E_HEAD			11			/* 写头出错 */
#define		E_BODY			12			/* 写体出错 */
#define		E_RECORD		13			/* 一条记录没达到最小单词个数*/
#define		E_FMT			14			/* 一条记录中的单词个数已经超过最大单词数*/
#define		E_RMB			15			/* 去掉文件中空格出错 */
#define		E_FMEM			16			/* 内存不够*/
#define		E_MER_R			17			/* 合并记录出错 */
#define		E_REG			18			/* 注册函数出错*/
#define		E_KEY			19			/* 输入的主键不对*/


#ifdef WIN32
#include <windows.h>
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



