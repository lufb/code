/**
 *	base/gcs_wt.c
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
#include "native.h"
#include "gcs_wt.h"
#include "gcs_http.h"
#include "gcs_errno.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: base/gcs_wt.c,"
	"v 1.00 2013/11/05 11:58:40 yangling Exp $ (LBL)";
#endif

#pragma pack(1)

typedef struct _ipos_head {
	unsigned char	magic;		/*	数据包开始标志*/
	unsigned char	ver;		/*	版本号*/
	unsigned int	aft_len;	/*	编码之后数据头后的发送数据包长度*/
	unsigned char	type;		/*	数据包类型*/
	unsigned short	bef_len;	/*	编码之前的数据头后的数据长度*/
	unsigned char	chk;		/*	校验码*/
}IPOS_HEAD;

typedef struct _uc_check {
	unsigned char	method;		/*	认证方法*/
	unsigned char	usr_len;	/*	用户名长度*/
	unsigned char	pass_len;	/*	密码长度*/
	unsigned char	chk;		/*	校验码*/
}UC_CHECK;

typedef struct _uc_checkreply {
	unsigned char	reply;		/*	CU_CERTIFYREPLYTYPE*/
	unsigned char	chk;		/*	校验码*/
}UC_CHECKREPLY;

typedef struct _uc_clientinfo {
	unsigned int	ipv4;		/*	IP地址*/
	unsigned short	port;		/*	端口*/
	unsigned char	chk;		/*	校验码*/
}UC_CLIENTINFO;

#pragma pack()

#define TYPE_CU_CHECKREPLY		1
#define TYPE_CU_CONNECTREPLY	3
#define TYPE_UC_CLIENTINFO		6

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
	OUT void *package, IN size_t pkg_size)
{
	char			temp[sizeof(IPOS_HEAD)
							+ sizeof(UC_CHECKREPLY)];
	IPOS_HEAD		*header = (IPOS_HEAD*)temp;
	UC_CHECKREPLY	*reply	= (UC_CHECKREPLY*)(header + 1);

	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);	

	header->magic	= '#';
	header->ver		= 0x02;
	header->aft_len = 0;
	header->type	= TYPE_CU_CHECKREPLY;
	header->bef_len	= sizeof(UC_CHECKREPLY);
	header->chk		= 0;

	reply->reply	= 0;
	reply->chk		= 0;

	return build_transfer_package(
			c_unitno, c_tunnel, temp,
			sizeof(IPOS_HEAD) + sizeof(UC_CHECKREPLY),
			package, pkg_size);
}

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
	OUT void *package, IN size_t pkg_size)
{
	char			temp[sizeof(IPOS_HEAD)
							+ sizeof(UC_CHECKREPLY)];
	IPOS_HEAD		*header = (IPOS_HEAD*)temp;
	UC_CHECKREPLY	*reply	= (UC_CHECKREPLY*)(header + 1);

	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);	

	header->magic	= '#';
	header->ver		= 0x02;
	header->aft_len = 0;
	header->type	= TYPE_CU_CONNECTREPLY;
	header->bef_len	= sizeof(UC_CHECKREPLY);
	header->chk		= 0;

	reply->reply	= 0;
	reply->chk		= 0;

	return build_transfer_package(
			c_unitno, c_tunnel, temp,
			sizeof(IPOS_HEAD) + sizeof(UC_CHECKREPLY),
			package, pkg_size);
}

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
	OUT void *package, IN size_t pkg_size)
{
	IPOS_HEAD		*header = (IPOS_HEAD*)package;
	UC_CLIENTINFO	*addr	= (UC_CLIENTINFO*)(header + 1);

	if(pkg_size != MAX_HTTP_PKG_SIZE)
		return gen_errno(0, GCEINVAL);	
	
	header->magic	= '#';
	header->ver		= 0x02;
	header->aft_len = 0;
	header->type	= TYPE_UC_CLIENTINFO;
	header->bef_len	= sizeof(UC_CLIENTINFO);
	header->chk		= 0;
	
	addr->ipv4		= ipv4;
	addr->port		= port;
	addr->chk		= 0;
	
	return sizeof(IPOS_HEAD) + sizeof(UC_CLIENTINFO);
}

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
	IN const void *buffer, IN size_t size)
{
	int	rc;

	UNREFERENCED_PARAMETER(buffer);

	if(wtp->size1 && size) {
		if(size >= wtp->size1) {
			size -= wtp->size1;
			wtp->size1 = 0;

			if(IS_NOT_NULL(wtp->wt_pkg1_cbk)) {
				rc = wtp->wt_pkg1_cbk(wtp->user);
				if(rc)
					return rc;
			}	
		} else {
			wtp->size1 -= size;
			return 0;
		}
	}

	if(wtp->size2 && size) {
		if(size >= wtp->size2) {
			size -= wtp->size2;
			wtp->size2 = 0;

			if(IS_NOT_NULL(wtp->wt_pkg2_cbk)) {
				rc = wtp->wt_pkg2_cbk(wtp->user);
				if(rc)
					return rc;
			}
		} else {
			wtp->size2 -= size;
			return 0;
		}
	}

	return size;
}

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
	IN int	(*wt_pkg2_cbk)(IN void *))
{
	wt_parser_reset(wtp);
	wtp->user			= user;
	wtp->wt_pkg1_cbk	= wt_pkg1_cbk;
	wtp->wt_pkg2_cbk	= wt_pkg2_cbk;
}

/**
 *	wt_parser_create - 创建WT的解析引擎
 *
 *	@wtp:			WT解析引擎
 *
 *	return
 *		无
 */
void
wt_parser_create(IN struct wt_parser *wtp)
{
	wtp->size1 = sizeof(IPOS_HEAD) +
					sizeof(UC_CHECK);
	wtp->size2 = sizeof(IPOS_HEAD) +
					sizeof(UC_CLIENTINFO);
}

