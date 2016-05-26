/**
 *	include/gcs_conn.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__CONN_HEADER__
#define __CONN_HEADER__

#include "bits/gcs_conn.h"
#include "gcs_link.h"

/**
 *	global_conn_module_init - ȫ�����Ӵ���ģ���ʼ��
 *
 *	return
 *		0			�ɹ�
 *		!0			ʧ��
 */
int
global_conn_module_init();

/**
 *	gcs_aio_commit_connect - ��ɫͨ������һ���첽����������
 *
 *	@addr:		��ַ��Ϣ(big endian)
 *	@port:		�˿���Ϣ(big endian)
 *	@launcher:	������
 *	@_unitno:	�첽��Ϣ(��Ԫ��)
 *	@_tunnel:	�첽��Ϣ(�����)
 *	@_action:	�첽��Ϣ(������)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int gcs_aio_commit_connect(
	IN unsigned int ip,
	IN unsigned short port,
	IN linkno_t launcher,
	IN unsigned short _unitno,
	IN unsigned short _tunnel,
	IN unsigned short _action);

/**
 *	make_conn_module_ctrl_buffer - ����connģ��Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_conn_module_ctrl_buffer(
	IN char *buffer, IN size_t size);

#endif	/*	__CONN_HEADER__*/