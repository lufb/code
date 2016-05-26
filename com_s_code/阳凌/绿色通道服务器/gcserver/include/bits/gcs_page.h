/**
 *	include/bits/gcs_page.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__BITS_PAGE_HEADER__
#define	__BITS_PAGE_HEADER__

#include "features.h"
#include "native.h"
#include "pthread.h"
#include "list.h"

/**
 *	ÿ���ڴ�ҳ�Ĵ�С8192KB
 */
#define		MPAGE_SIZE			0x2000

#define		FOR_TX_USE			0
#define		FOX_RX_USE			1

/**
 *	�ڴ�ҳͷ
 */
struct page_head {
	struct list_head			list;			/*	���ڵ�*/	
	size_t						size;			/*	�����������ݵ�ʵ�ʴ�СBytes*/
	char				*		buffer;			/*	����������ָ��*/
};

#endif	/*	__BITS_PAGE_HEADER__*/
