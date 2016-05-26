/**
 *	include/gcs_config.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__CONFIG_HEADER__
#define	__CONFIG_HEADER__

#include "bits/gcs_config.h"

/**
 *	global_config_init - 全局配置初始化
 *
 *	return
 *		无
 */
int
global_config_init();

/**
 *	ref_config_object - 引用配置对象
 *
 *	return
 *		配置对象
 */
struct gcs_config*
ref_config_object();

/**
 *	lookup_nat_rules - 查找NAT规则
 *
 *	@src_ipv4:		源IP地址
 *	@src_port:		源端口(big endian)
 *
 *	return
 *		NULL		没找到
 *		!NULL		找到了
 */
struct gcs_nat *
lookup_nat_rules(
	IN char *src_ipv4, IN unsigned short src_port);

/**
 *	load_key_value - 提取配置项
 *
 *	@name:			配置文件名
 *	@section:		段名
 *	@key:			KEY名
 *	@def:			DEFAULT值
 *	@bret:			返回缓冲区
 *	@size:			返回缓冲区大小
 *
 *	return
 *		无
 */
void
load_key_value(IN char *name, IN char *section,
	IN char *key, IN char *def, OUT char *bret, OUT size_t size);

#endif	/*	__CONFIG_HEADER__*/