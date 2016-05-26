/*
 *	lib/crypto/crc16.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	crc16算法
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__CRC16_HEADER__
#define	__CRC16_HEADER__

#include <stddef.h>
#include "features.h"

/**
 *	crc16 - CRC16算法
 *
 *	@buffer:		缓冲区
 *	@size:			数据大小
 *	@lasts:			接力运算(上次的值)
 *
 *	return
 *		CRC16
 */
unsigned short
crc16(IN const char *buffer, IN size_t size, IN unsigned short lasts);

#endif	/*	__CRC16_HEADER__*/