/*
 *	lib/crypto/crc16.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	crc16�㷨
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__CRC16_HEADER__
#define	__CRC16_HEADER__

#include <stddef.h>
#include "features.h"

/**
 *	crc16 - CRC16�㷨
 *
 *	@buffer:		������
 *	@size:			���ݴ�С
 *	@lasts:			��������(�ϴε�ֵ)
 *
 *	return
 *		CRC16
 */
unsigned short
crc16(IN const char *buffer, IN size_t size, IN unsigned short lasts);

#endif	/*	__CRC16_HEADER__*/