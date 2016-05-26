/**
 *	include/config.h
 *
 *	Copyright (C) 2012 ���� <yl.tienon@gmail.com>
 *
 *	��������û�������ص����ݽṹ
 *
 *	�޸���ʷ:
 *
 *	2013-01-18 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */

#ifndef __CONFIG_HEADER__
#define __CONFIG_HEADER__

struct _ulimit {
	unsigned short		proxy_limit;			/*	�������������������*/
	unsigned short		upgrade_limit;			/*	��������������������*/
};

/**
 *	config_init - ���ó�ʼ��
 *
 *	return
 *		��
 */
void config_init();

/**
 *	get_ulimit - �õ�������Ϣ
 *
 *	return
 *		������Ϣ���ݽṹ
 */
struct _ulimit *get_ulimit();

#endif	/**	__CONFIG_HEADER__*/