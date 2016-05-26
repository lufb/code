/**
 *	include/gcs_http.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__HTTP_HEADER__
#define	__HTTP_HEADER__

#include "bits/gcs_http.h"

/**
 *	build_hello_package - �����������ݰ�
 *
 *	@s_channo:			����������
 *	@c_channo:			�ͷ��������
 *	@errorcode:			�������
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
 */
int
build_hello_package(
	IN unsigned short s_channo, IN unsigned short c_channo,
	IN unsigned int	errorcode, OUT void *package, IN size_t pkg_size);

/**
 *	build_open_package - �������������ݰ�
 *
 *	@action:			��������(GcType)
 *	@c_unitno:			��Ԫ��
 *	@c_tunnel:			�����
 *	@errorcode:			�������
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
 */
int
build_open_package(IN unsigned short action,
	IN unsigned short c_unitno, IN unsigned short c_tunnel,
	IN unsigned int	errorcode, OUT void *package, IN size_t pkg_size);

/**
 *	build_transfer_package - ��������ת�����ݰ�
 *
 *	@c_unitno:			��Ԫ��
 *	@c_tunnel:			�����
 *	@data:				ת��������
 *	@data_size:			ת�������ݴ�С
 *	@package:			���ݰ�
 *	@pkg_size:			���ݰ��ĺϷ���С
 *
 *	return
 *		<	0			ʧ��
 *		==	0			(����)ʧ��
 *		>	0			package��С
 */
int
build_transfer_package(
	IN unsigned short c_unitno, IN unsigned short c_tunnel,
	IN void *data, IN size_t data_size,
	OUT void *package, IN size_t pkg_size);

#endif	/*	__HTTP_HEADER__*/