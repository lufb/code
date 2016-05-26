/**
 *	init/gcs_version.c
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
#include <stdio.h>
#include "gcs_version.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: init/gcs_version.c,"
	"v 1.00 2013/11/01 14:43:40 yangling Exp $ (LBL)";
#endif

/**
 *	�¸�ʽ�İ汾��
 */
static unsigned int	major = 1;	/**	���汾��*/
static unsigned int	minor = 0;	/**	�ΰ汾��*/
static unsigned int build = 1;	/**	�����*/
static unsigned int patch = 1;	/**	������*/

/**
 *	��ͳǬ¡��ʽ�汾��
 */
static unsigned int version = ((((unsigned int)200 << 16) + 1));

/**
 *	��������
 *
 *	����Ұ�Ե�ʨ���Ƿ��޴��ԭ�İ���.
 *	ϣ�����ܳ�Ϊ����������İ���
 */
static char *develop_code = "lion";

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
global_public_version_str(IN char *ver, IN size_t size)
{
	UNREFERENCED_PARAMETER(size);

	sprintf(ver,
#ifdef _DEBUG
	"V%2.2f B%03d - %s(D)",
#else
	"V%2.2f B%03d - %s(R)",
#endif
	(double)((version & 0xFFFF0000) >> 16) / 100, 
	version & 0x0000FFFF, develop_code);

	return ver;
}

/**
 *	global_public_version_int - ����Ǭ¡��ʽ�İ汾
 *
 *	return
 *		�汾�ŵ�������
 */
unsigned int
global_public_version_int()
{
	return version;
}

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
global_private_version_str(IN char *ver, IN size_t size)
{
	UNREFERENCED_PARAMETER(size);

	sprintf(ver,
#ifdef _DEBUG
	"%d.%d.%d.%d - %s D",
#else
	"%d.%d.%d.%d - %s R",
#endif
	major, minor, build, patch, develop_code);

	return ver;
}


