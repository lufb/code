/**
 *	include/gcs_smif.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__SMIF_HEADER__
#define	__SMIF_HEADER__

#include "bits/gcs_smif.h"

/**
 *	global_smif_module_init - ȫ�ֳ�ʼ��smifģ��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
global_smif_module_init();

/**
 *	smif_ctrl_cmd_print - �� SM ת����������Ľ��(������)
 *
 *	@cmd_no:	�������
 *	@fmt:		�����(���� printf�庯��)
 *
 *	return
 *		��
 */
void
smif_ctrl_cmd_print(
	IN unsigned int cmd_no, IN const char *fmt, ...);

/**
 *	smif_ctrl_cmd_println - �� SM ת����������Ľ��(����)
 *						
 *	@cmd_no:	�������
 *	@fmt:		�����(���� printf�庯��)
 *
 *	return
 *		��
 */
void
smif_ctrl_cmd_println(
	IN unsigned int cmd_no, IN const char *fmt, ...);

/**
 *	write_log - �� SM ������־(�ӿں���)
 *
 *	@moduile:	ģ��ID,ֵ���ڵ�һ�����ӿ�Э�̵�ʱ��ƽ̨�����
 *	@log_type:	��־����
 *	@log_level: ��־�ȼ�
 *	@fmt:		�����(���� printf�庯��)
 *
 *	return
 *		�޷���ֵ
 */
void
write_log(
	IN int module_id, IN unsigned char log_type,
	IN unsigned char log_level, IN const char *fmt, ...);

/**
 *	smif_refresh_module_status - �� SM ˢ��ģ��ķ���״̬
 *
 *	@module_id:		ģ��ID
 *	@c:				״̬
 *
 *	return
 *		>=		0	�ɹ�
 *		<		0	ʧ��
 */
int
smif_refresh_module_status(IN int module_id, IN char c);

/**
 *	smif_refresh_module_verbose_status - �� SM ˢ��ģ���״̬��Ϣ
 *
 *	@module_id:		ģ��ID
 *	@si:			״̬������
 *	@size:			״̬��������С
 *
 *	return
 *		>=		0	�ɹ�
 *		<		0	ʧ��
 */
int
smif_refresh_module_verbose_status(
	IN int module_id, IN char *si, IN size_t size);

#endif	/*	__SMIF_HEADER__*/