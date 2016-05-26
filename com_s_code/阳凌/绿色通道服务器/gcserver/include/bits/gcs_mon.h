/**
 *	include/bits/gcs_mon.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_MON_HEADER__
#define __BITS_MON_HEADER__

#include "features.h"

#pragma pack(1)
struct gcs_mon {
	unsigned int		magic_skin:24;	/*  �ṹ��ħ��*/
	unsigned int		version_skin:8;	/*  �ṹ�İ汾��*/
	unsigned int		uptime;			/*	��������*/
	unsigned int		version;		/*	�汾��*/
	unsigned short		nr_sleeping;	/*	˯���߳���*/
	unsigned short		nr_running;		/*	�����߳���*/
	unsigned short		nr_zombie;		/*	��ʬ�߳���*/
	unsigned short		nr_unknown;		/*	δ֪�߳���*/
	unsigned short		nr_total;		/*	�ܵ��߳���*/
	unsigned short		nr_frags;		/*	�ڴ��Ƭ��*/
	unsigned int		nr_pages;		/*	����ʹ����*/
	unsigned int		nr_tx_use;		/*	��������ʹ����*/
	unsigned int		nr_rx_use;		/*	��������ʹ����*/
	unsigned int		nr_ot_use;		/*	δ֪����ʹ����*/
	unsigned short		nr_srv_links;	/*	SERVER������*/
	unsigned short		nr_cli_links;	/*	CLIENT������*/
	unsigned __int64	srv_received;	/*	SERVER�����ֽ���*/
	unsigned __int64	srv_sent;		/*	SERVER�����ֽ���*/
	unsigned __int64	srv_committed;	/*	SERVER�ύ�ֽ���*/
	unsigned __int64	cli_received;	/*	SERVER�����ֽ���*/
	unsigned __int64	cli_sent;		/*	SERVER�����ֽ���*/
	unsigned __int64	cli_committed;	/*	SERVER�ύ�ֽ���*/
};
#pragma pack()

#endif	/*	__BITS_MON_HEADER__*/