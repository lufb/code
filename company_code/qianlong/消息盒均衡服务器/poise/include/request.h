/**
* include/request.h
*
*	向平台注册的头文件
*
*	2012-09-20 - 首次创建
*		            LUFUBO
*/
#include "if.h"

#ifndef	__REQUEST_HEADER__
#define	__REQUEST_HEADER__


extern	struct mosp_interface	*mbi_sp_interface;		/*	平台的接口的指针*/
extern	unsigned short			demo_module_no;			/*	平台分配给我的模块号*/



#define	_BUILD_MPS(_mps, _main, _child, _lnk_no,			\
	_seqno, _pg_id, _session_id)			\
	do {												\
	(_mps)->main		= (_main);					\
	(_mps)->child		= (_child);					\
	(_mps)->link_no		= (_lnk_no);				\
	(_mps)->seqno		= (_seqno);					\
	(_mps)->page_id		= (_pg_id);					\
	if((_session_id) == 0)							\
				(_mps)->session_id							\
				= ((_lnk_no) & 0xFFFF) + 1; \
				else											\
				(_mps)->session_id							\
				= _session_id;				\
	} while( 0 )

#ifdef  __cplusplus
extern  "C" {
#endif
	
	/**
	*	do_request_134_51 - 请求代理报告固有信息
	*
	*	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
	*	@swap:			同步处理函数该参数永NULL,请千万不要使用
	*	@swap_sz:		同步处理函数该参数永0,无任何意义
	*	@data:			数据内容(协议体的数据)
	*	@data_sz:		数据的大小(协议体数据的大小)
		*/
	int	SPCALLBACK do_request134_51(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_136_51 - 请求升级报告固有信息
	*
	*	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
	*	@swap:			同步处理函数该参数永NULL,请千万不要使用
	*	@swap_sz:		同步处理函数该参数永0,无任何意义
	*	@data:			数据内容(协议体的数据)
	*	@data_sz:		数据的大小(协议体数据的大小)
		*/
	int	SPCALLBACK do_request136_51(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_137_51 - 请求客户端请求列表信息
	*
	*	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
	*	@swap:			同步处理函数该参数永NULL,请千万不要使用
	*	@swap_sz:		同步处理函数该参数永0,无任何意义
	*	@data:			数据内容(协议体的数据)
	*	@data_sz:		数据的大小(协议体数据的大小)
		*/
	int	SPCALLBACK do_request137_51(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_134_153 - 请求代理报告状态信息
	*
	*	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
	*	@swap:			同步处理函数该参数永NULL,请千万不要使用
	*	@swap_sz:		同步处理函数该参数永0,无任何意义
	*	@data:			数据内容(协议体的数据)
	*	@data_sz:		数据的大小(协议体数据的大小)
		*/
	int	SPCALLBACK do_request134_153(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_136_153 - 请求升级报告状态信息
	*
	*	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
	*	@swap:			同步处理函数该参数永NULL,请千万不要使用
	*	@swap_sz:		同步处理函数该参数永0,无任何意义
	*	@data:			数据内容(协议体的数据)
	*	@data_sz:		数据的大小(协议体数据的大小)
		*/
	int	SPCALLBACK do_request136_153(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_134_152 - 请求代理注销信息
	*
	*	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
	*	@swap:			同步处理函数该参数永NULL,请千万不要使用
	*	@swap_sz:		同步处理函数该参数永0,无任何意义
	*	@data:			数据内容(协议体的数据)
	*	@data_sz:		数据的大小(协议体数据的大小)
		*/
	int	SPCALLBACK do_request134_152(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

	/**
	*	do_request_136_152 - 请求升级注销信息
	*
	*	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
	*	@swap:			同步处理函数该参数永NULL,请千万不要使用
	*	@swap_sz:		同步处理函数该参数永0,无任何意义
	*	@data:			数据内容(协议体的数据)
	*	@data_sz:		数据的大小(协议体数据的大小)
		*/
	int	SPCALLBACK do_request136_152(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);

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
									 IN const char *data, IN size_t data_sz);


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
									 IN const char *data, IN size_t data_sz);


	
		/**
		*	do_logout - 登出的请求处理函数
		*
		*	@msg_hdr:		消息头(该消息头屏蔽了所有的协议差异)
		*	@swap:			同步处理函数该参数永NULL,请千万不要使用
		*	@swap_sz:		同步处理函数该参数永0,无任何意义
		*	@data:			数据内容(协议体的数据)
		*	@data_sz:		数据的大小(协议体数据的大小)
	*/
	int	SPCALLBACK do_logout(
		IN const struct msg_header *msg_hdr,
		IN const char *swap, IN size_t swap_sz,
		IN const char *data, IN size_t data_sz);
	
		/**
		*	do_linkmsg - 处理链路的断开与连接的信息
		*
		*	@msg_hdr:		消息头
		*
		*	return
		*		0			成功
		*		!0			失败
	*/
	int	SPCALLBACK do_linkmsg(IN struct msg_header *msg_hdr);
	



	
#ifdef __cplusplus
}
#endif
#endif	/*	__SAMPLE_HEADER__*/