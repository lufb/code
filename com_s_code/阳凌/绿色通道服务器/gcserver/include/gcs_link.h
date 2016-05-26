/**
 *	include/gcs_link.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__LINK_HEADER__
#define __LINK_HEADER__

#include "bits/gcs_link.h"
#include "bits/gcs_mon.h"

/**
 *	global_link_module_init - 全局初始化LINK模块
 *
 *	return
 *		!0			失败
 *		0			成功
 */
int
global_link_module_init();

/**
 *	gcs_peek_link - 偷窥是否还有可用的LINKNO资源
 *
 *	return
 *		!0		还有
 *		0		没有
 */
int gcs_peek_link();

/**
 *	alloc_link_for_server - 分配一个SERVER链路
 *
 *	@peer_no:		对等LINKNO
 *	@s:				SOCKET描述符
 *	@action:		动作码(GcType)
 *	@ipv4:			目标IP地址(big endian)
 *	@port:			目标端口(big endian)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
alloc_link_for_server(IN linkno_t peer_no,
	IN int s, IN unsigned short action,
	IN unsigned int ipv4, IN unsigned short port);

/**
 *	alloc_link_for_client - 分配一个CLIENT链路
 *
 *	@s:				SOCKET描述符
 *	@ipv4:			目标IP地址(big endian)
 *	@port:			目标端口(big endian)
 *	@from:			来至端口(big endian)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
alloc_link_for_client(
	IN int s, IN unsigned int ipv4,
	IN unsigned short port, IN unsigned short from);

/**
 *	gcs_aio_complete_sent - GCS 异步IO 成功完成发送
 *
 *	@link_no:		LINKNO
 *	@size:			成功发送的字节
 *
 *	return
 *		无
 */
void
gcs_aio_complete_sent(IN linkno_t link_no, IN size_t size);

/**
 *	gcs_aio_commit_send - GCS 异步IO 提交发送
 *
 *	@link_no:		LINKNO
 *	@buffer:		数据缓冲区
 *	@size:			数据缓冲区的大小
 *
 *	return
 *		>	0		成功
 *		==	0		保留(不可能返回)
 *		<	0		失败
 */
int
gcs_aio_commit_send(IN linkno_t link_no,
	IN void *buffer, IN size_t size);

/**
 *	gcs_aio_complete_recv - GCS 异步IO 成功完成接收
 *
 *	@wtab:			单词表
 *	@link_no:		LINKNO
 *	@size:			成功接收的字节
 *
 *	return
 *		无
 */
void gcs_aio_complete_recv(
	IN struct word_table *wtab,
	IN linkno_t link_no, IN size_t size);

/**
 *	gcs_free_link - 释放一个LINK槽
 *
 *	@link_no:		LINKNO
 *
 *	return
 *		无
 */
void gcs_free_link(IN linkno_t link_no);

/**
 *	gcs_hard_close_link 硬关闭一个LINK
 *
 *	@link_no:	LINKNO
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
gcs_hard_close_link(IN linkno_t link_no);

/**
 *	gcs_logical_close_link 逻辑关闭一个LINK
 *
 *	@link_no:	LINKNO
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
gcs_logical_close_link(IN linkno_t link_no);

/**
 *	make_link_module_ctrl_buffer - 构建link模块的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_link_module_ctrl_buffer(
	IN char *buffer, IN size_t size);

/**
 *	refresh_link_mon - 刷新link监控数据
 *
 *	@mon:		监控数据结构
 *
 *	return
 *		无
 */
void
refresh_link_mon(IN struct gcs_mon *mon);

/**
 *	check_all_connection_timeout - 检查所有的连接的超时
 *
 *	return
 *		无
 */
void
check_all_connection_timeout();

#endif	/*	__LINK_HEADER__*/