/**
 *	include/gcs_smif.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__SMIF_HEADER__
#define	__SMIF_HEADER__

#include "bits/gcs_smif.h"

/**
 *	global_smif_module_init - 全局初始化smif模块
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
global_smif_module_init();

/**
 *	smif_ctrl_cmd_print - 向 SM 转发控制命令的结果(非最后包)
 *
 *	@cmd_no:	命令序号
 *	@fmt:		变参数(类型 printf族函数)
 *
 *	return
 *		无
 */
void
smif_ctrl_cmd_print(
	IN unsigned int cmd_no, IN const char *fmt, ...);

/**
 *	smif_ctrl_cmd_println - 向 SM 转发控制命令的结果(最后包)
 *						
 *	@cmd_no:	命令序号
 *	@fmt:		变参数(类型 printf族函数)
 *
 *	return
 *		无
 */
void
smif_ctrl_cmd_println(
	IN unsigned int cmd_no, IN const char *fmt, ...);

/**
 *	write_log - 向 SM 推送日志(接口函数)
 *
 *	@moduile:	模块ID,值是在第一次做接口协商的时候平台分配的
 *	@log_type:	日志类型
 *	@log_level: 日志等级
 *	@fmt:		变参数(类型 printf族函数)
 *
 *	return
 *		无返回值
 */
void
write_log(
	IN int module_id, IN unsigned char log_type,
	IN unsigned char log_level, IN const char *fmt, ...);

/**
 *	smif_refresh_module_status - 向 SM 刷新模块的服务状态
 *
 *	@module_id:		模块ID
 *	@c:				状态
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
int
smif_refresh_module_status(IN int module_id, IN char c);

/**
 *	smif_refresh_module_verbose_status - 向 SM 刷新模块的状态信息
 *
 *	@module_id:		模块ID
 *	@si:			状态缓冲区
 *	@size:			状态缓冲区大小
 *
 *	return
 *		>=		0	成功
 *		<		0	失败
 */
int
smif_refresh_module_verbose_status(
	IN int module_id, IN char *si, IN size_t size);

#endif	/*	__SMIF_HEADER__*/