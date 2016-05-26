/**
 *	include/gcs_http.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__HTTP_HEADER__
#define	__HTTP_HEADER__

#include "bits/gcs_http.h"

/**
 *	build_hello_package - 构建握手数据包
 *
 *	@s_channo:			服务端隧道号
 *	@c_channo:			客服端隧道号
 *	@errorcode:			错误代码
 *	@package:			数据包
 *	@pkg_size:			数据包的合法大小
 *
 *	return
 *		<	0			失败
 *		==	0			(保留)失败
 *		>	0			package大小
 */
int
build_hello_package(
	IN unsigned short s_channo, IN unsigned short c_channo,
	IN unsigned int	errorcode, OUT void *package, IN size_t pkg_size);

/**
 *	build_open_package - 构建打开连接数据包
 *
 *	@action:			动作代码(GcType)
 *	@c_unitno:			单元号
 *	@c_tunnel:			隧道号
 *	@errorcode:			错误代码
 *	@package:			数据包
 *	@pkg_size:			数据包的合法大小
 *
 *	return
 *		<	0			失败
 *		==	0			(保留)失败
 *		>	0			package大小
 */
int
build_open_package(IN unsigned short action,
	IN unsigned short c_unitno, IN unsigned short c_tunnel,
	IN unsigned int	errorcode, OUT void *package, IN size_t pkg_size);

/**
 *	build_transfer_package - 构建数据转发数据包
 *
 *	@c_unitno:			单元号
 *	@c_tunnel:			隧道号
 *	@data:				转发的数据
 *	@data_size:			转发的数据大小
 *	@package:			数据包
 *	@pkg_size:			数据包的合法大小
 *
 *	return
 *		<	0			失败
 *		==	0			(保留)失败
 *		>	0			package大小
 */
int
build_transfer_package(
	IN unsigned short c_unitno, IN unsigned short c_tunnel,
	IN void *data, IN size_t data_size,
	OUT void *package, IN size_t pkg_size);

#endif	/*	__HTTP_HEADER__*/