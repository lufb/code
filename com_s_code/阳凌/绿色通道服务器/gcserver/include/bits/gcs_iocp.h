/**
 *	include/bits/gcs_iocp.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__BITS_IOCP_HEADER__
#define __BITS_IOCP_HEADER__

#include "features.h"

struct iocp_model {
	HANDLE					handle;				/*	IOCP ���*/
	unsigned short			nr_wk_ths;			/*	�����߳���*/
	unsigned short			padding;			/*	����*/
};

#endif	/*	__BITS_IOCP_HEADER__*/