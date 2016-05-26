/**
 *	include/config.h
 *
 *	Copyright (C) 2012 阳凌 <yl.tienon@gmail.com>
 *
 *	定义关于用户配置相关的数据结构
 *
 *	修改历史:
 *
 *	2013-01-18 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef __CONFIG_HEADER__
#define __CONFIG_HEADER__

struct _ulimit {
	unsigned short		proxy_limit;			/*	代理服务器的连接限制*/
	unsigned short		upgrade_limit;			/*	升级服务器的连接限制*/
};

/**
 *	config_init - 配置初始化
 *
 *	return
 *		无
 */
void config_init();

/**
 *	get_ulimit - 得到限制信息
 *
 *	return
 *		限制信息数据结构
 */
struct _ulimit *get_ulimit();

#endif	/**	__CONFIG_HEADER__*/