/**
 *	include/gcs_config.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__CONFIG_HEADER__
#define	__CONFIG_HEADER__

#include "bits/gcs_config.h"

/**
 *	global_config_init - ȫ�����ó�ʼ��
 *
 *	return
 *		��
 */
int
global_config_init();

/**
 *	ref_config_object - �������ö���
 *
 *	return
 *		���ö���
 */
struct gcs_config*
ref_config_object();

/**
 *	lookup_nat_rules - ����NAT����
 *
 *	@src_ipv4:		ԴIP��ַ
 *	@src_port:		Դ�˿�(big endian)
 *
 *	return
 *		NULL		û�ҵ�
 *		!NULL		�ҵ���
 */
struct gcs_nat *
lookup_nat_rules(
	IN char *src_ipv4, IN unsigned short src_port);

/**
 *	load_key_value - ��ȡ������
 *
 *	@name:			�����ļ���
 *	@section:		����
 *	@key:			KEY��
 *	@def:			DEFAULTֵ
 *	@bret:			���ػ�����
 *	@size:			���ػ�������С
 *
 *	return
 *		��
 */
void
load_key_value(IN char *name, IN char *section,
	IN char *key, IN char *def, OUT char *bret, OUT size_t size);

#endif	/*	__CONFIG_HEADER__*/