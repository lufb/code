/**
 *	include/bits/gcs_iocp.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__BITS_IOCP_HEADER__
#define __BITS_IOCP_HEADER__

#include "features.h"

struct iocp_model {
	HANDLE					handle;				/*	IOCP 句柄*/
	unsigned short			nr_wk_ths;			/*	工作线程数*/
	unsigned short			padding;			/*	保留*/
};

#endif	/*	__BITS_IOCP_HEADER__*/