/**
 *	include/gcs_ctrl.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__CTRL_HEADER__
#define	__CTRL_HEADER__

/**
 *	global_ctrl_command_init - ȫ�ֳ�ʼ����������
 *
 *	return
 *		��
 */
void
global_ctrl_command_init();

/**
 *	global_ctrl_command_entry - ����̨�������
 *
 *	@cmd_no:		�������к�
 *	@argc:			�������
 *	@argv:			�������
 *
 *	return
 *		��
 */
void
global_ctrl_command_entry(
	IN unsigned int cmd_no, IN int argc, IN char *argv[]);

#endif	/*	__CTRL_HEADER__*/