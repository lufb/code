/**
 *	include/bits/gcs_page.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */

#ifndef	__BITS_PAGE_HEADER__
#define	__BITS_PAGE_HEADER__

#include "features.h"
#include "native.h"
#include "pthread.h"
#include "list.h"

/**
 *	每个内存页的大小8192KB
 */
#define		MPAGE_SIZE			0x2000

#define		FOR_TX_USE			0
#define		FOX_RX_USE			1

/**
 *	内存页头
 */
struct page_head {
	struct list_head			list;			/*	链节点*/	
	size_t						size;			/*	缓冲区中数据的实际大小Bytes*/
	char				*		buffer;			/*	缓冲区数据指针*/
};

#endif	/*	__BITS_PAGE_HEADER__*/
