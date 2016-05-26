/**
 *	include/gcs_wt.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__WT_HEADER__
#define	__WT_HEADER__

#include "bits/gcs_wt.h"

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
	OUT void *package, IN size_t pkg_size);

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
	OUT void *package, IN size_t pkg_size);

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
	OUT void *package, IN size_t pkg_size);

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
	IN int	(*wt_pkg2_cbk)(IN void *));

/**
 *	wt_parser_create - ����WT�Ľ�������
 *
 *	@wtp:			WT��������
 *
 *	return
 *		��
 */
void
wt_parser_create(IN struct wt_parser *wtp);

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
	IN const void *buffer, IN size_t size);

#endif	/*	__WT_HEADER__*/