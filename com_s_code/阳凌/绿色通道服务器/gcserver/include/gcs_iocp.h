/**
 *	include/gcs_iocp.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__IOCP_HEADER__
#define __IOCP_HEADER__

#include "bits/gcs_iocp.h"

/**
 *	ref_iocp_model_object - ����IOCPģ�Ͷ���
 *
 *	return
 *		��
 */
struct iocp_model *
ref_iocp_model_object();

/**
 *	global_iocp_module_init - ȫ�ֳ�ʼ��IOCPģ��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
global_iocp_module_init();


#endif	/*	__IOCP_HEADER__*/