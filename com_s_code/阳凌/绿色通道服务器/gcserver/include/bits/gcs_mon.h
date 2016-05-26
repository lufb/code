/**
 *	include/bits/gcs_mon.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_MON_HEADER__
#define __BITS_MON_HEADER__

#include "features.h"

#pragma pack(1)
struct gcs_mon {
	unsigned int		magic_skin:24;	/*  结构的魔数*/
	unsigned int		version_skin:8;	/*  结构的版本号*/
	unsigned int		uptime;			/*	启动秒数*/
	unsigned int		version;		/*	版本号*/
	unsigned short		nr_sleeping;	/*	睡眠线程数*/
	unsigned short		nr_running;		/*	运行线程数*/
	unsigned short		nr_zombie;		/*	僵尸线程数*/
	unsigned short		nr_unknown;		/*	未知线程数*/
	unsigned short		nr_total;		/*	总的线程数*/
	unsigned short		nr_frags;		/*	内存块片数*/
	unsigned int		nr_pages;		/*	正在使用数*/
	unsigned int		nr_tx_use;		/*	发送正在使用数*/
	unsigned int		nr_rx_use;		/*	接收正在使用数*/
	unsigned int		nr_ot_use;		/*	未知正在使用数*/
	unsigned short		nr_srv_links;	/*	SERVER连接数*/
	unsigned short		nr_cli_links;	/*	CLIENT连接数*/
	unsigned __int64	srv_received;	/*	SERVER接收字节数*/
	unsigned __int64	srv_sent;		/*	SERVER发送字节数*/
	unsigned __int64	srv_committed;	/*	SERVER提交字节数*/
	unsigned __int64	cli_received;	/*	SERVER接收字节数*/
	unsigned __int64	cli_sent;		/*	SERVER发送字节数*/
	unsigned __int64	cli_committed;	/*	SERVER提交字节数*/
};
#pragma pack()

#endif	/*	__BITS_MON_HEADER__*/