/**
 *	include/gcs_conn.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__CONN_HEADER__
#define __CONN_HEADER__

#include "bits/gcs_conn.h"
#include "gcs_link.h"

/**
 *	global_conn_module_init - 全局连接处理模块初始化
 *
 *	return
 *		0			成功
 *		!0			失败
 */
int
global_conn_module_init();

/**
 *	gcs_aio_commit_connect - 绿色通道发送一个异步的连接请求
 *
 *	@addr:		地址信息(big endian)
 *	@port:		端口信息(big endian)
 *	@launcher:	发起者
 *	@_unitno:	异步信息(单元号)
 *	@_tunnel:	异步信息(隧道号)
 *	@_action:	异步信息(动作码)
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int gcs_aio_commit_connect(
	IN unsigned int ip,
	IN unsigned short port,
	IN linkno_t launcher,
	IN unsigned short _unitno,
	IN unsigned short _tunnel,
	IN unsigned short _action);

/**
 *	make_conn_module_ctrl_buffer - 构建conn模块的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_conn_module_ctrl_buffer(
	IN char *buffer, IN size_t size);

#endif	/*	__CONN_HEADER__*/