/**
 *	include/gcs_lsnr.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__LSNR_HEADER__
#define __LSNR_HEADER__

#include	"bits/gcs_lsnr.h"

/**
 *	make_lsnr_module_ctrl_buffer - 构建lsnr模块的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_lsnr_module_ctrl_buffer(
	IN char *buffer, IN size_t size);

/**
 *	global_lsnr_module_init - 全局初始化LSNR模块
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
global_lsnr_module_init();

#endif	/*	__LSNR_HEADER__*/