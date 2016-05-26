/**
 *	include/gcs_page.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef __PAGE_HEADER__
#define	__PAGE_HEADER__

#include "bits/gcs_page.h"
#include "bits/gcs_mon.h"

/**
 *	global_page_module_init - ȫ�ֳ�ʼ��ҳģ��
 *
 *	return
 *		0		�ɹ�
 *		!0		ʧ��
 */
int
global_page_module_init();

/**
 *	getpage - ����һ���ڴ�ҳ��
 *
 *	@purpose:		ʹ��Ŀ��
 *
 *	return
 *		NULL	-	ʧ��
 *		!NULL	-	�ɹ�
 */
struct page_head * getpage(IN int purpose);

/**
 *	putpage - �黹һ���ڴ�ҳ��
 *
 *	@pg:	�ڴ�ҳͷ
 *	@purpose:		ʹ��Ŀ��
 *
 *	return
 *		��
 */
void	putpage(IN struct page_head *pg, IN int purpose);

/**
 *	pgread - ��һ���ڴ�ҳ��
 *
 *	@pg:		�ڴ�ҳͷ
 *	@pos:		׼����ҳ�ڵ�ƫ��λ��
 *	@buffer:	׼������Ŀ�껺����
 *	@size:		׼����������ݴ�С(Bytes)
 *
 *	return
 *		>=	0	�ɹ������ֽ���
 *		<	0	ʧ��
 *
 *	remark:
 *		��������Ҫ��֤ buffer�Ļ�������Сһ����������size�����ݿռ��
 *	������ܻ��������ķ���
 */
int
pgread(
	IN struct page_head *pg, IN size_t pos, IN char *buffer, IN size_t size);

/**
 *	pgwrite - дһ���ڴ�ҳ��
 *
 *	@pg:		�ڴ�ҳͷ
 *	@start:		׼��д��ҳ��ƫ��λ��
 *	@buffer:	׼��д��Ŀ�껺����
 *	@size:		׼��д�����ݴ�С(Bytes)
 *
 *	return
 *		>=	0	�ɹ�д����ֽ���
 *		<	0	ʧ��
 */
int
pgwrite(
	IN struct page_head *pg, IN size_t pos, IN char *buffer, IN size_t size);

/**
 *	make_page_ctrl_buffer - ����page����Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_page_ctrl_buffer(
	IN char *buffer, IN size_t size);

/**
 *	refresh_page_mon - ˢ��page�������
 *
 *	@mon:		������ݽṹ
 *
 *	return
 *		��
 */
void
refresh_page_mon(IN struct gcs_mon *mon);

#endif	/*	__PAGE_HEADER__*/