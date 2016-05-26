/**
 * src/request.c
 *
 *	请求模块: 处理请求的回调函数
 *
 *	2012-09-20 - 首次创建
 *		            LUFUBO
 */

#include "lock.h"
#include "request.h"
#include "protocol.h"
#include "stdio.h"
#include "poise.h"
#include "link.h"

extern SRV_INFO	*gSrvInfo;//代理服务器信息数组

extern struct list_head agentListHead;//代理服务器链表头
extern struct list_head updateListHead;//升级服务器链表头
extern struct futex_mutex gLock;


/**
 *	do_request_134_51 - 向平台请求代理报告固有信息的处理函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		>0			成功
 *		<=			失败
 */
int	SPCALLBACK do_request134_51(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{
	return process134_51(msg_hdr, data, data_sz);
}

/**
 *	do_request_136_51 - 向平台请求升级报告固有信息的处理函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		>0			成功
 *		<=0			失败
 */
int	SPCALLBACK do_request136_51(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process136_51(msg_hdr, data, data_sz);
}

/**
 *	do_request_134_153 - 向平台请求代理刷状态数据的处理函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int	SPCALLBACK do_request134_153(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process134_153(msg_hdr, data, data_sz);
}

/**
 *	do_request_136_153 - 向平台请求升级报告状态数据的处理函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int	SPCALLBACK do_request136_153(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process136_153(msg_hdr, data, data_sz);	
}

/**
 *	do_request_134_152 - 向平台请求代理注销的数据处理函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int	SPCALLBACK do_request134_152(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process134_152(msg_hdr, data, data_sz);
}

/**
 *	do_request_136_152 - 向平台请求升级注销的处理函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int	SPCALLBACK do_request136_152(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process136_152(msg_hdr, data, data_sz);
}

/**
 *	do_request134_154 - 向平台请求代理报告服务状态函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int	SPCALLBACK do_request134_154(
								 IN const struct msg_header *msg_hdr,
								 IN const char *swap, IN size_t swap_sz,
								 IN const char *data, IN size_t data_sz)
{	
	return process134_154(msg_hdr, data, data_sz);
}

/**
 *	do_request136_154 - 向平台请求升级报告服务状态函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int	SPCALLBACK do_request136_154(
								 IN const struct msg_header *msg_hdr,
								 IN const char *swap, IN size_t swap_sz,
								 IN const char *data, IN size_t data_sz)
{	
	return process136_154(msg_hdr, data, data_sz);
}

/**
 *	do_request_137_51 - 向平台请求客户端请求代理服务器列表与升级服务器列表的处理函数
 *
 *	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
 *	@swap:			同步处理函数该参数永NULL,请千万不要使用
 *	@swap_sz:		同步处理函数该参数永0,无任何意义
 *	@data:			数据内容(协议体的数据)
 *	@data_sz:		数据的大小(协议体数据的大小)
 *
 *	return
 *		>0			成功
 *		<=0			失败
 */
int	SPCALLBACK do_request137_51(
	IN const struct msg_header *msg_hdr,
	IN const char *swap, IN size_t swap_sz,
	IN const char *data, IN size_t data_sz)
{	
	return process137_51(msg_hdr, data, data_sz);
}


/**
 *	do_linkmsg - 处理链路的断开与连接的信息
 *
 *	@msg_hdr:		消息头
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int	SPCALLBACK do_linkmsg(IN struct msg_header *msg_hdr)
{
	struct _util_ops	*util_ops = &(mbi_sp_interface->util_ops);
	unsigned int		LinkNo = 0;

	switch(msg_hdr->msgtype)
	{
	case MSG_ACTIVE_CONNECT:			// 主动连接建立（与上级服务器建立的连接）,可能不会使用到
		break;
	case MSG_ACTIVE_CONN_CLOSE:			// 主动连接关闭,（与上级服务器建立的连接）,可能不会使用到
		break;
	case MSG_PASSIVE_CONNECT:			// 被动连接建立（与客户端建立的连接）
#ifdef _DEBUG
		util_ops->write_log(
					demo_module_no,
					LOG_TYPE_INFO,
					LOG_LEVEL_DEVELOPERS, "客服端连接到服务器linkno:%d", msg_hdr->link_no);
#endif
		break;
	case MSG_PASSIVE_CONN_CLOSE:		// 被动连接关闭（与客户端建立的连接）
#ifdef _DEBUG
		util_ops->write_log(
					demo_module_no,
					LOG_TYPE_INFO,
					LOG_LEVEL_DEVELOPERS, "客服端与服务器断开连接linkno:%d", msg_hdr->link_no);
#endif
		do_passive_conn_close(msg_hdr);	
		break;
	default:
		break;
	}

	return 0;
}




