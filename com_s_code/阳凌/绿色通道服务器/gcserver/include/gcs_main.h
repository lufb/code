/**
 *	include/gcs_main.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__MAIN_HEADER__
#define	__MAIN_HEADER__

#include "bits/gcs_main.h"

/**
 *	alloc_gcs_thread_object - ����һ��GCS�̶߳���
 *
 *	@name:		����
 *
 *	return
 *		NULL	ʧ��
 *		!NULL	�ɹ�
 *
 *	remark
 *		û�м���,��Ϊʵ���в�����race-condition���
 */
struct gcs_thread*
alloc_gcs_thread_object(IN char *name);

/**
 *	local_gcs_thread_init - �̳߳�ʼ��(ÿ���߳̽�����һ��)
 *
 *	@gth:		�߳�������
 *
 *	return
 *		��
 */
void
local_gcs_thread_init(IN struct gcs_thread *gth);

/**
 *	make_ts_ctrl_buffer - ����ts����Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_ts_ctrl_buffer(
	IN char *buffer, IN size_t size);

/**
 *	make_dts_ctrl_buffer - ����dts����Ŀ���̨�������
 *
 *	@buffer:		������
 *	@size:			���������ֵ
 *
 *	return
 *		��
 */
void
make_dts_ctrl_buffer(
	IN char *buffer, IN size_t size);

#endif	/*	__MAIN_HEADER__*/