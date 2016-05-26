/**
 *	init/gcs_version.c
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
#include <stdio.h>
#include "gcs_version.h"

#ifndef lint
static const char rcsid[] _U_ =
	"@(#) $Id: init/gcs_version.c,"
	"v 1.00 2013/11/01 14:43:40 yangling Exp $ (LBL)";
#endif

/**
 *	新格式的版本号
 */
static unsigned int	major = 1;	/**	主版本号*/
static unsigned int	minor = 0;	/**	次版本号*/
static unsigned int build = 1;	/**	编译号*/
static unsigned int patch = 1;	/**	补丁号*/

/**
 *	传统乾隆格式版本号
 */
static unsigned int version = ((((unsigned int)200 << 16) + 1));

/**
 *	开发代号
 *
 *	充满野性的狮子是非洲大草原的霸王.
 *	希望他能成为代理服务器的霸王
 */
static char *develop_code = "lion";

/**
 *	global_public_version_str - 返回乾隆格式的版本
 *
 *	@ver:		缓冲区
 *	@size:		缓冲区大小
 *
 *	return
 *		Vx.yy Bzz D/R 格式字符串
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
 *	global_public_version_int - 返回乾隆格式的版本
 *
 *	return
 *		版本号的整形数
 */
unsigned int
global_public_version_int()
{
	return version;
}

/**
 *	global_private_version_str - 返回新格式的版本
 *
 *	@ver:		缓冲区
 *	@size:		缓冲区大小
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


