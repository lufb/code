/**
 *	include/gcs_iocp.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__IOCP_HEADER__
#define __IOCP_HEADER__

#include "bits/gcs_iocp.h"

/**
 *	ref_iocp_model_object - 引用IOCP模型对象
 *
 *	return
 *		无
 */
struct iocp_model *
ref_iocp_model_object();

/**
 *	global_iocp_module_init - 全局初始化IOCP模块
 *
 *	return
 *		0		成功
 *		!0		失败
 */
int
global_iocp_module_init();


#endif	/*	__IOCP_HEADER__*/