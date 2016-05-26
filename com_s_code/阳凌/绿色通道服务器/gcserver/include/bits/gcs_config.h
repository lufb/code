/**
 *	include/bits/gcs_config.h
 *
 *	Copyright (C) 2013 阳凌 <yl.tienon@gmail.com>
 *
 *	修改历史:
 *
 *	2013-10-15 - 首次创建
 *
 *                     阳凌  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_CONFIG_HEADER__
#define __BITS_CONFIG_HEADER__

/**
 *	最多同时监听的端口数量(本宏不能修改)
 */
#define	NR_LSNR_PORT	64
#define	CONFIG_VALUE_LENGTH	1024

struct gcs_config {
	unsigned short		lsnr_port[NR_LSNR_PORT];
	unsigned short		nr_lsnrs;		/*	有效的监听端口个数*/
	unsigned short		core_ths;		/*	核心工作线程数量*/
	unsigned short		conn_ths;		/*	连接工作线程数量*/
	unsigned short		nr_links;		/*	最大连接数*/
	unsigned int		bytes_per_link;	/*	每个链路的缓冲区大小*/
	unsigned short		log_level;		/*	日志等级*/
	unsigned short		rst_close;		/*	RST关闭链路*/
	unsigned short		reuseaddr;		/*	地址重绑定*/
	unsigned short		timeout;		/*	超时检查*/
};

#define	MAX_DOMAIN_LENGTH	255
struct gcs_nat {
	char	*src_ptr;
	char	*dst_ptr;
	char	src_addr[MAX_DOMAIN_LENGTH + 1];	/*	源地址*/
	char	dst_addr[MAX_DOMAIN_LENGTH + 1];	/*	目标地址*/
	unsigned short src_port;					/*	源端口*/
	unsigned short dst_port;					/*	目标端口*/
};

#endif	/*	__BITS_CONFIG_HEADER__*/