#ifndef _ERROR_DEFINE_H_
#define _ERROR_DEFINE_H_
#include <stdio.h>
/*
	定义相应的错误码
*/
#define		E_SUCESS			0			/* 正常，没出错*/
#define		E_OPTION			1			/* 读配置出错*/
#define		E_RCV				2			/*客户端关闭*/
#define		E_MYABORT			3			/*断言出错*/
#define		E_TIMEOUT			4			/*收数据超时*/
#define		E_USERNAME			5			/*用户名或者密码不对*/
#define		E_ERRID				6			/*消息ID出错*/
#define		E_LINK_TIMEOUT		7			/*链路无数据状态超时*/
#define		E_RESERV			8			/*校验保留字节不对*/
#define		E_DATA				9			/*本来不应该收到数据的，结果还收到了数据*/
#define		E_CLI_UNBIND		10			/*客户端发unbind协议，主动断开*/
#define		E_MEM_LEAK			11			/*内存不足*/
#define		E_CHECK				12			/*校验数据出错*/
#define		E_TEXT				13			/*字符串内容不满足要求*/
#define		E_MSG_LEN			14			/*消息的长度不对*/
#define		E_LIST_EMPTY		15			/*链表是空的*/
#define		E_BLOCK_SEND		16			/*阻塞发数据出错*/
#define		E_BLOCK_RECV		17			/*阻塞收数据出错*/
#define		E_HEAD_SIZE			18			/*消息头中的长度不对*/
#define		E_RSP				19			/*消息类型不为回应*/



#define		E_UNKNNOWN			34			/*回给用户的未知错误类型*/

/*回应的错误码如下所示*/
/*
0	无错误，命令正确接收
1	非法登录，如登录名、口令出错、登录名与口令不符等。
2	重复登录，如在同一TCP/IP连接中连续两次以上请求登录。
3	连接过多，指单个节点要求同时建立的连接数过多。
4	登录类型错，指bind命令中的logintype字段出错。
5	参数格式错，指命令中参数值与参数类型不符或与协议规定的范围不符。
6	非法手机号码，协议中所有手机号码字段出现非86130号码或手机号码前未加"86"时都应报错。
7	消息ID错
8	信息长度错
9	非法序列号，包括序列号重复、序列号格式错误等
10	非法操作GNS
11	节点忙，指本节点存储队列满或其他原因，暂时不能提供服务的情况
21	目的地址不可达，指路由表存在路由且消息路由正确但被路由的节点暂时不能提供服务的情况
22	路由错，指路由表存在路由但消息路由出错的情况，如转错SMG等
23	路由不存在，指消息路由的节点在路由表中不存在
24	计费号码无效，鉴权不成功时反馈的错误信息
25	用户不能通信（如不在服务区、未开机等情况）
26	手机内存不足
27	手机不支持短消息
28	手机接收短消息出现错误
29	不知道的用户
30	不提供此功能
31	非法设备
32	系统失败
33	短信中心队列满
34	卢富波定义的不晓得什么错误的错误码
注：错误码1-20所指错误一般在各类命令的应答中用到，21-32所指错误一般在report命令中用到。

*/



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

#define PRINT_ERR_NO(_errcode_)				\
	printf("syserror[%d] usrerror[%d]\n",	\
	GET_SYS_ERR(_errcode_),					\
	GET_USER_ERR(_errcode_))



#endif
