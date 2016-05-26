/**
 *	base/gcs_wt.c
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
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
	unsigned char	magic;		/*	���ݰ���ʼ��־*/
	unsigned char	ver;		/*	�汾��*/
	unsigned int	aft_len;	/*	����֮������ͷ��ķ������ݰ�����*/
	unsigned char	type;		/*	���ݰ�����*/
	unsigned short	bef_len;	/*	����֮ǰ������ͷ������ݳ���*/
	unsigned char	chk;		/*	У����*/
}IPOS_HEAD;

typedef struct _uc_check {
	unsigned char	method;		/*	��֤����*/
	unsigned char	usr_len;	/*	�û�������*/
	unsigned char	pass_len;	/*	���볤��*/
	unsigned char	chk;		/*	У����*/
}UC_CHECK;

typedef struct _uc_checkreply {
	unsigned char	reply;		/*	CU_CERTIFYREPLYTYPE*/
	unsigned char	chk;		/*	У����*/
}UC_CHECKREPLY;

typedef struct _uc_clientinfo {
	unsigned int	ipv4;		/*	IP��ַ*/
	unsigned short	port;		/*	�˿�*/
	unsigned char	chk;		/*	У����*/
}UC_CLIENTINFO;

#pragma pack()

#define TYPE_CU_CHECKREPLY		1
#define TYPE_CU_CONNECTREPLY	3
#define TYPE_UC_CLIENTINFO		6

/**
 *	build_wt_package1 - ����WT���ݰ�1
 *
 *	@c_unitno:			��Ԫ��
 *	@c_tunnel:			�����
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
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
 *	build_wt_package2 - ����WT���ݰ�2
 *
 *	@c_unitno:			��Ԫ��
 *	@c_tunnel:			�����
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
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
 *	build_wt_packagen - ����WT���ݰ�n(IP�����)
 *
 *	@ipv4:			IP��ַ(big endian)
 *	@port:			�˿���Ϣ(big endian)
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
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
 *	do_wt_parse - ����WT���Ľ���
 *
 *	@wtp:			WT��������
 *	@buffer:		���ݻ�����
 *	@size:			���ݻ������Ĵ�С
 *
 *	return
 *		>	0		��������������ֽ�
 *		==	0		���ݲ�������
 *		<	0		���������г��ִ���
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
 *	wt_parser_init - WT���������ʼ��
 *
 *	@wtp:			WT��������
 *	@user:			�û����
 *	@wt_pkg1_cbk:	WT��һ�����Ļص�����
 *	@wt_pkg2_cbk:	WT�ڶ������Ļص�����
 *
 *	return
 *		��
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
 *	wt_parser_create - ����WT�Ľ�������
 *
 *	@wtp:			WT��������
 *
 *	return
 *		��
 */
void
wt_parser_create(IN struct wt_parser *wtp)
{
	wtp->size1 = sizeof(IPOS_HEAD) +
					sizeof(UC_CHECK);
	wtp->size2 = sizeof(IPOS_HEAD) +
					sizeof(UC_CLIENTINFO);
}

