/**
 *	include/gcs_main.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__MAIN_HEADER__
#define	__MAIN_HEADER__

#include "bits/gcs_main.h"

/**
 *	alloc_gcs_thread_object - 分配一个GCS线程对象
 *
 *	@name:		名字
 *
 *	return
 *		NULL	失败
 *		!NULL	成功
 *
 *	remark
 *		没有加锁,因为实际中不会有race-condition情况
 */
struct gcs_thread*
alloc_gcs_thread_object(IN char *name);

/**
 *	local_gcs_thread_init - 线程初始化(每个线程仅调用一次)
 *
 *	@gth:		线程描述符
 *
 *	return
 *		无
 */
void
local_gcs_thread_init(IN struct gcs_thread *gth);

/**
 *	make_ts_ctrl_buffer - 构建ts命令的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_ts_ctrl_buffer(
	IN char *buffer, IN size_t size);

/**
 *	make_dts_ctrl_buffer - 构建dts命令的控制台命令缓冲区
 *
 *	@buffer:		缓冲区
 *	@size:			缓冲区最大值
 *
 *	return
 *		无
 */
void
make_dts_ctrl_buffer(
	IN char *buffer, IN size_t size);

#endif	/*	__MAIN_HEADER__*/