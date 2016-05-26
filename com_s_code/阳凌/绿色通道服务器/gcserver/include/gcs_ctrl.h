/**
 *	include/gcs_ctrl.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__CTRL_HEADER__
#define	__CTRL_HEADER__

/**
 *	global_ctrl_command_init - 全局初始化控制命令
 *
 *	return
 *		无
 */
void
global_ctrl_command_init();

/**
 *	global_ctrl_command_entry - 控制台命令入口
 *
 *	@cmd_no:		命令序列号
 *	@argc:			命令个数
 *	@argv:			命令参数
 *
 *	return
 *		无
 */
void
global_ctrl_command_entry(
	IN unsigned int cmd_no, IN int argc, IN char *argv[]);

#endif	/*	__CTRL_HEADER__*/