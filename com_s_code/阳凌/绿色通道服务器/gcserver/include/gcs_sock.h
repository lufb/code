/**
 *	include/gcs_socket.c
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__SOCKET_HEADER__
#define __SOCKET_HEADER__

/**
 *	ne_startup - 初始化网络环境
 *
 *	return
 *		无
 */
void ne_startup();

/**
 *	ne_cleanup - 销毁网络环境
 *
 *	return
 *		无
 */
void ne_cleanup();

/**
 *	ne_setreuseaddr - 设置地址重用
 *
 *	@s:			SOCKET描述符
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
ne_setreuseaddr(IN int s);

/**
 *	ne_close - 关闭SOCKET
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int ne_close(IN int s);

/**
 *	ne_fast_close - 快速关闭SOCKET RST包
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int ne_fast_close(IN int s);

/**
 *	ne_getsock_errno - 根据SOCKET描述符,得到指定SOCKET的最后一次错误码
 *
 *	@s:		SOCKET描述符
 *
 *	return
 *		-1			出错
 *		otherwise	错误代码
 */
int
ne_getsock_errno(IN int s);

/**
 *	ne_setnonblocking - 设置SOCKET非阻塞
 *
 *	@s:		SOCKET描述符
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
ne_setnonblocking(IN int s);

/**
 *	ne_setblocking - 设置SOCKET阻塞
 *
 *	@s:		SOCKET描述符
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
ne_setblocking(IN int s);

/**
 *	ne_inet_ntopx - 线程安全的IP地址和端口转换成字符串
 *
 *	@ip:			IP地址(big endian)
 *	@port:			端口(big endian)
 *	@addr:			地址缓冲区
 *	@addr_len:		地址缓冲区长度
 *
 *	return
 *		地址信息
 */
char*
ne_inet_ntopx(IN unsigned int ip,
	IN unsigned short port, OUT char *addr, IN size_t addr_len);

/**
 *	ne_connect2 - 连接服务器
 *
 *	@ip:				IP地址(big endian)
 *	@port:				PORT端口(big endian)
 *
 *	return:
 *		> 0		成功
 *		<=0		失败
 */
int ne_connect2(
	IN unsigned int ip, IN unsigned short port);

/**
 *	ne_asyn_connect - 异步连接
 *
 *	@ip:				IP地址(big endian)
 *	@port:				PORT端口(big endian)
 *	@s:					创建好的SOCKET描述符
 *
 *	return
 *		>	0			成功(需要继续通过select来探测SOCKET是否已经可写)
 *		==	0			成功(不需要通过select来探测)
 *		<	0			失败
 */
int
ne_asyn_connect(IN unsigned int ip,
	IN unsigned short port, IN int *s);

#endif	/*	__SOCKET_HEADER__*/
