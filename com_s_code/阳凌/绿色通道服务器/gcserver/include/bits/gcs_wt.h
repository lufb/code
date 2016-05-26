/**
 *	include/bits/gcs_wt.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_WT_HEADER__
#define __BITS_WT_HEADER__

#include "features.h"
#include <stddef.h>

struct wt_parser {
	void			*user;		/*	用户句柄*/
	int		(*wt_pkg1_cbk)(IN void *);
	int		(*wt_pkg2_cbk)(IN void *);
	size_t			size1;		/*	第一个包剩余解析的字节*/
	size_t			size2;		/*	第二个包剩余解析的字节*/
};

#define	wt_parser_reset(_wtp_)	\
	do {						\
		(_wtp_)->size1	= 0;	\
		(_wtp_)->size2	= 0;	\
	} while(0)

#endif	/*	__BITS_WT_HEADER__*/