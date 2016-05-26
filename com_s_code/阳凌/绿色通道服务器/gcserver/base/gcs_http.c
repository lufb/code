/**
 *	base/gcs_http.c
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#include "features.h"
#include "gcs_page.h"
#include "gcs_errno.h"
#include "gcs_http.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: base/gcs_http.c,"
	"v 1.00 2013/10/29 11:25:40 yangling Exp $ (LBL)";
#endif

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
	IN unsigned int	errorcode, OUT void *package, IN size_t pkg_size)
{
	int		rc;

	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);

#define	HTTP_HELLO_FMT					\
		"HTTP/1.1 200 OK\r\n"			\
		"GCType: %u\r\n"				\
		"SRVCHAN: %u\r\n"				\
		"GCCHNO: %u\r\n"				\
		"GCErCD: %u\r\n"				\
		"Connection: Keep-Alive\r\n"	\
		"Content-Length: 0\r\n"			\
		"\r\n""\r\n\r\n"

	rc = sprintf(package, HTTP_HELLO_FMT,
					GCA_HELLO, s_channo,
					c_channo, errorcode);

	return rc;
}

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
	IN unsigned int	errorcode, OUT void *package, IN size_t pkg_size)
{
	int		rc;
	
	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);

#define	HTTP_OPEN_FMT					\
		"HTTP/1.1 200 OK\r\n"			\
		"GCType: %u\r\n"				\
		"GCUNNO: %u\r\n"				\
		"GCLNKNO: %u\r\n"				\
		"GCErCD: %u\r\n"				\
		"Connection: Keep-Alive\r\n"	\
		"Content-Length: 0\r\n"			\
		"\r\n""\r\n\r\n"

	rc =  sprintf(package, HTTP_OPEN_FMT,
					action, c_unitno,
					c_tunnel, errorcode);

	return rc;
}

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
	OUT void *package, IN size_t pkg_size)
{
	int		rc;

	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);

	if(data_size > MPAGE_SIZE)
		return gen_errno(0, GCEINVAL);

#define	HTTP_TRANSFER_FMT			\
	"HTTP/1.1 200 OK\r\n"			\
	"GCType: %u\r\n"				\
	"GCUNNO: %u\r\n"				\
	"GCLNKNO: %u\r\n"				\
	"Connection: Keep-Alive\r\n"	\
	"Content-Length: %u\r\n"		\
	"\r\n"

	rc = sprintf(package, HTTP_TRANSFER_FMT,
					GCA_TRANSFER, c_unitno,
					c_tunnel, data_size);

	memcpy((char*)package + rc, data, data_size);
	rc += data_size;

	memcpy((char*)package + rc, "\r\n\r\n", 4);
	rc += 4;

	return rc;
}
