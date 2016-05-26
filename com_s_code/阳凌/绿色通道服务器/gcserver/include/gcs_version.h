/**
 *	include/gcs_version.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__VERSION_HEADER__
#define	__VERSION_HEADER__

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
global_public_version_str(IN char *ver, IN size_t size);

/**
 *	global_public_version_int - 返回乾隆格式的版本
 *
 *	return
 *		版本号的整形数
 */
unsigned int
global_public_version_int();

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
global_private_version_str(IN char *ver, IN size_t size);

#endif	/*	__VERSION_HEADER__*/