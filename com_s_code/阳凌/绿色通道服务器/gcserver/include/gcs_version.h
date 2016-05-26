/**
 *	include/gcs_version.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__VERSION_HEADER__
#define	__VERSION_HEADER__

/**
 *	global_public_version_str - ����Ǭ¡��ʽ�İ汾
 *
 *	@ver:		������
 *	@size:		��������С
 *
 *	return
 *		Vx.yy Bzz D/R ��ʽ�ַ���
 */
char *
global_public_version_str(IN char *ver, IN size_t size);

/**
 *	global_public_version_int - ����Ǭ¡��ʽ�İ汾
 *
 *	return
 *		�汾�ŵ�������
 */
unsigned int
global_public_version_int();

/**
 *	global_private_version_str - �����¸�ʽ�İ汾
 *
 *	@ver:		������
 *	@size:		��������С
 *
 *	return
 *		major.minor.build.patch D/R
 */
char *
global_private_version_str(IN char *ver, IN size_t size);

#endif	/*	__VERSION_HEADER__*/