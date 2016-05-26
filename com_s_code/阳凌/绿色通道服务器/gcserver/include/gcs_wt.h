/**
 *	include/gcs_wt.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__WT_HEADER__
#define	__WT_HEADER__

#include "bits/gcs_wt.h"

/**
 *	build_wt_package1 - 构建WT数据包1
 *
 *	@c_unitno:			单元号
 *	@c_tunnel:			隧道号
 *	@package:			数据包
 *	@pkg_size:			数据包的合法大小
 *
 *	return
 *		<	0			失败
 *		==	0			(保留)失败
 *		>	0			package大小
 */
int
build_wt_package1(
	IN unsigned short c_unitno,
	IN unsigned short c_tunnel,
	OUT void *package, IN size_t pkg_size);

/**
 *	build_wt_package2 - 构建WT数据包2
 *
 *	@c_unitno:			单元号
 *	@c_tunnel:			隧道号
 *	@package:			数据包
 *	@pkg_size:			数据包的合法大小
 *
 *	return
 *		<	0			失败
 *		==	0			(保留)失败
 *		>	0			package大小
 */
int
build_wt_package2(
	IN unsigned short c_unitno,
	IN unsigned short c_tunnel,
	OUT void *package, IN size_t pkg_size);

/**
 *	build_wt_packagen - 构建WT数据包n(IP报告包)
 *
 *	@ipv4:			IP地址(big endian)
 *	@port:			端口信息(big endian)
 *	@package:			数据包
 *	@pkg_size:			数据包的合法大小
 *
 *	return
 *		<	0			失败
 *		==	0			(保留)失败
 *		>	0			package大小
 */
int
build_wt_packagen(
	IN unsigned int ipv4, IN unsigned short port,
	OUT void *package, IN size_t pkg_size);

/**
 *	wt_parser_init - WT解析引擎初始化
 *
 *	@wtp:			WT解析引擎
 *	@user:			用户句柄
 *	@wt_pkg1_cbk:	WT第一个包的回调函数
 *	@wt_pkg2_cbk:	WT第二个包的回调函数
 *
 *	return
 *		无
 */
void
wt_parser_init(
	IN struct wt_parser *wtp, IN void *user,
	IN int	(*wt_pkg1_cbk)(IN void *),
	IN int	(*wt_pkg2_cbk)(IN void *));

/**
 *	wt_parser_create - 创建WT的解析引擎
 *
 *	@wtp:			WT解析引擎
 *
 *	return
 *		无
 */
void
wt_parser_create(IN struct wt_parser *wtp);

/**
 *	do_wt_parse - 处理WT包的解析
 *
 *	@wtp:			WT解析引擎
 *	@buffer:		数据缓冲区
 *	@size:			数据缓冲区的大小
 *
 *	return
 *		>	0		不能吞入的数据字节
 *		==	0		数据不够吞入
 *		<	0		解析过程中出现错误
 */
int
do_wt_parse(IN struct wt_parser *wtp,
	IN const void *buffer, IN size_t size);

#endif	/*	__WT_HEADER__*/