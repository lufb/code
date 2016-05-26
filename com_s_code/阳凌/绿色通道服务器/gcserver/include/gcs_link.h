/**
 *	include/gcs_link.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef	__LINK_HEADER__
#define __LINK_HEADER__

#include "bits/gcs_link.h"
#include "bits/gcs_mon.h"

/**
 *	global_link_module_init - ȫ�ֳ�ʼ��LINKģ��
 *
 *	return
 *		!0			ʧ��
 *		0			�ɹ�
 */
int
global_link_module_init();

/**
 *	gcs_peek_link - ͵���Ƿ��п��õ�LINKNO��Դ
 *
 *	return
 *		!0		����
 *		0		û��
 */
int gcs_peek_link();

/**
 *	alloc_link_for_server - ����һ��SERVER��·
 *
 *	@peer_no:		�Ե�LINKNO
 *	@s:				SOCKET������
 *	@action:		������(GcType)
 *	@ipv4:			Ŀ��IP��ַ(big endian)
 *	@port:			Ŀ��˿�(big endian)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
alloc_link_for_server(IN linkno_t peer_no,
	IN int s, IN unsigned short action,
	IN unsigned int ipv4, IN unsigned short port);

/**
 *	alloc_link_for_client - ����һ��CLIENT��·
 *
 *	@s:				SOCKET������
 *	@ipv4:			Ŀ��IP��ַ(big endian)
 *	@port:			Ŀ��˿�(big endian)
 *	@from:			�����˿�(big endian)
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
alloc_link_for_client(
	IN int s, IN unsigned int ipv4,
	IN unsigned short port, IN unsigned short from);

/**
 *	gcs_aio_complete_sent - GCS �첽IO �ɹ���ɷ���
 *
 *	@link_no:		LINKNO
 *	@size:			�ɹ����͵��ֽ�
 *
 *	return
 *		��
 */
void
gcs_aio_complete_sent(IN linkno_t link_no, IN size_t size);

/**
 *	gcs_aio_commit_send - GCS �첽IO �ύ����
 *
 *	@link_no:		LINKNO
 *	@buffer:		���ݻ�����
 *	@size:			���ݻ������Ĵ�С
 *
 *	return
 *		>	0		�ɹ�
 *		==	0		����(�����ܷ���)
 *		<	0		ʧ��
 */
int
gcs_aio_commit_send(IN linkno_t link_no,
	IN void *buffer, IN size_t size);

/**
 *	gcs_aio_complete_recv - GCS �첽IO �ɹ���ɽ���
 *
 *	@wtab:			���ʱ�
 *	@link_no:		LINKNO
 *	@size:			�ɹ����յ��ֽ�
 *
 *	return
 *		��
 */
void gcs_aio_complete_recv(
	IN struct word_table *wtab,
	IN linkno_t link_no, IN size_t size);

/**
 *	gcs_free_link - �ͷ�һ��LINK��
 *
 *	@link_no:		LINKNO
 *
 *	return
 *		��
 */
void gcs_free_link(IN linkno_t link_no);

/**
 *	gcs_hard_close_link Ӳ�ر�һ��LINK
 *
 *	@link_no:	LINKNO
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
gcs_hard_close_link(IN linkno_t link_no);

/**
 *	gcs_logical_close_link �߼��ر�һ��LINK
 *
 *	@link_no:	LINKNO
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
gcs_logical_close_link(IN linkno_t link_no);

/**
 *	make_link_module_ctrl_buffer - ����linkģ��Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_link_module_ctrl_buffer(
	IN char *buffer, IN size_t size);

/**
 *	refresh_link_mon - ˢ��link�������
 *
 *	@mon:		������ݽṹ
 *
 *	return
 *		��
 */
void
refresh_link_mon(IN struct gcs_mon *mon);

/**
 *	check_all_connection_timeout - ������е����ӵĳ�ʱ
 *
 *	return
 *		��
 */
void
check_all_connection_timeout();

#endif	/*	__LINK_HEADER__*/