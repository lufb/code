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

#define		ESUCESS			0			/* 正常，没出错*/
#define		EOS				1			/*	操作系统错误*/
#define		EABORT			2			/* 断言出错*/			
#define		ELOGIC			3			/* 程序逻辑出错*/
#define		ECCLOSE			4			/* 客户端套接字关闭*/
#define		ESCLOSE			5			/* 服务端套接字关闭*/
#define		ESETSOCK		6			/* 设置套接字属性出错*/
#define		EFASTCLOSE		7			/* 需求快速关闭*/
#define		EBUFLESS		8			/* 缓冲区过小*/
#define		EENGSTATUS		9			/* 引擎状态出错*/
#define		EPORTISUSED		10			/* bind的端口被占用*/
#define		ELINKOVERFLOW	11			/* 存储监听的到的套接字缓冲区不够*/
#define		ELISTEMP		12			/* 对空链表进行了操作*/
#define		EWAITERROR		13			/* 等待线程出错*/
#define		ERERELEASE		14			/* 重复释放资源*/
#define		ELISTEMPTY		15			/* 链表为空*/
#define		ESELSTEERROR	16			/* 往select添加元素时状态出错*/
#define		EHANDFULL		17			/* Hand队列已满*/
#define		ECONNECT		18			/* CONNECT失败*/
#define		ESTATUS			19			/* 状态不对*/
#define		EPARAM			20			/* 参数出错*/
#define		ESOCKCLOSE		21			/* 套接字关闭 */
#define		ERCVTIMEOUT		22			/* 在特定时间没检测到套接字可读*/
#define		EDNS			23			/* DNS转换出错 */
#define		EHELLO			24			/* 收到的hello包有问题*/
#define		EHAND			25			/* 服务端回应的握手包有错(比如目标服务器不在)*/
#define		EPARASE			26			/* http解析数据出错 */
#define		EGCTYPE			27			/* 服务器返回的GcType有错*/
#define		EUSERPROTOCAL	28			/* 用户协议数据出错*/
#define		ECRETHREAD		29			/* 创建线程出错 */



/* 定义针对GcC支持代理的错误码基数*/
#define		E_AGENT_BASE		100

#define		E_PRO_HEAD			E_AGENT_BASE+1		/* 协议头带的特殊字符不正确 */		
#define		E_PRO_TYPE			E_AGENT_BASE+2		/* 客户端上报的代理类型不支持 */
#define		E_ERR_MAAGIC		E_AGENT_BASE+3		/* 客户上报上来的特殊标志不对 */

#define		E_ERR_NULLCHAR		E_AGENT_BASE+4		/* 发送的认证用户名或密码为空 */	/* 用户可收到信息 */
#define		E_ERR_HTTP			E_AGENT_BASE+5		/* 校验收到的http代理数据出错*/	
#define		E_ERR_SOCK5			E_AGENT_BASE+6		/* SOCK5认证时出错 */
#define		ERR_UNKNOW_ADDRTYPE E_AGENT_BASE+7		/* 未知地址类型（SOCKS5中会遇到）*/
#define		ERR_SERVICE_REJ		E_AGENT_BASE+8		/* 服务器拒绝服务（SOCKS5中会遇到）*/
#define		ERR_HTTP_RESPONSE	E_AGENT_BASE+8		/* http代理返回的代码为非200 */
#define		ERR_USER_BAUTH		E_AGENT_BASE+9		/* SOCK5代理需要认证，而用户设置为不用认证 */ /* 用户可收到信息 */

	
/* 定义 SOCK4的认证错误码*/
/* SOCK4认证返回值如下
90：授权代理
91：拒绝服务或失败
92：拒绝服务，由于代理服务器无法连接客户端认证
93：拒绝服务，客户端和认证提供的用户id不一致
*/
#define		ESOCK4_91				91
#define		ESOCK4_92				92
#define		ESOCK4_93				93
#define		ESOCK4_UNKNOWN			94

#define		ERR_UNKNOW_AUTH			95		/* 未知认证类型 */






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


/* 为了和老版本兼容的错误码(仅在用户获取特定的错误信息：比如端口占用时会用到)而对应定义的获取系统错误码宏*/
#define MERROR_SYSTEM				0x80000000					//操作系统错误
#define	USER_GET_LAST_ERR			(((GetLastError()) & 0x7FFFFFFF) + MERROR_SYSTEM)


#endif