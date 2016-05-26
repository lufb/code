/**
 *	include/gcs_mon.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__MON_HEADER__
#define	__MON_HEADER__

#include "bits/gcs_mon.h"

/**
 *	ref_gcs_mon_object - ����GCS�ļ�ض���
 *
 *	return
 *		GCS��ض���
 */
struct gcs_mon *
ref_gcs_mon_object();

/**
 * gcs_mon_module_init - GCS����ģ���ʼ��
 *
 *	return
 *		��
 */
void
gcs_mon_module_init();

#endif	/*	__MON_HEADER__*/