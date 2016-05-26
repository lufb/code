/**
 *	include/bits/gcs_wt.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_WT_HEADER__
#define __BITS_WT_HEADER__

#include "features.h"
#include <stddef.h>

struct wt_parser {
	void			*user;		/*	�û����*/
	int		(*wt_pkg1_cbk)(IN void *);
	int		(*wt_pkg2_cbk)(IN void *);
	size_t			size1;		/*	��һ����ʣ��������ֽ�*/
	size_t			size2;		/*	�ڶ�����ʣ��������ֽ�*/
};

#define	wt_parser_reset(_wtp_)	\
	do {						\
		(_wtp_)->size1	= 0;	\
		(_wtp_)->size2	= 0;	\
	} while(0)

#endif	/*	__BITS_WT_HEADER__*/