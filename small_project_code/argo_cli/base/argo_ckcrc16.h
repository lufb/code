/**
 *	argo_ckcrc16.h
 *
 *	Copyright (C) 2016 YangLing(yl.tienon@gmail.com)
 *
 *	Description:
 *
 *	Revision History:
 *
 *	2016-01-13 Created YangLing
 */

#ifndef __ARGO_CKCRC16_HEADER__
#define	__ARGO_CKCRC16_HEADER__


#include "stddef.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/**
 *	argo_ckcrc16_init - CRC16 init
 *
 *	return
 *		CRC16校验值
 */
unsigned short argo_ckcrc16_init(void);

/**
 *	argo_ckcrc16_update - CRC16 update
 *
 *	@crc16:			校验值
 *	@buff:			缓冲区
 *	@size:			缓冲区大小
 *
 *	return
 *		CRC16校验值
 */
unsigned short argo_ckcrc16_update(unsigned short crc16, const void * buff, size_t size);

/**
 *	argo_ckcrc16_final - CRC16 final
 *
 *	@crc16:			校验值
 *
 *	return
 *		CRC16校验值
 */
unsigned short argo_ckcrc16_final(unsigned short crc16);

/**
 *	argo_ckcrc16_calc - 计算CRC16 校验码
 *
 *	@buff:			缓冲区
 *	@size:			缓冲区大小
 *
 *	return
 *		CRC16校验值
 */
unsigned short argo_ckcrc16_calc(const void * buff, size_t size);

#ifdef __cplusplus
}
#endif
#endif // !__ARGO_CKCRC16_HEADER__
