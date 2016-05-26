/**
 *	include/gcs_lsnr.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__LSNR_HEADER__
#define __LSNR_HEADER__

#include	"bits/gcs_lsnr.h"

/**
 *	make_lsnr_module_ctrl_buffer - ����lsnrģ��Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_lsnr_module_ctrl_buffer(
	IN char *buffer, IN size_t size);

/**
 *	global_lsnr_module_init - ȫ�ֳ�ʼ��LSNRģ��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
global_lsnr_module_init();

#endif	/*	__LSNR_HEADER__*/