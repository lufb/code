/**
 *	include/bits/gcs_config.h
 *
 *	Copyright (C) 2013 ���� <yl.tienon@gmail.com>
 *
 *	�޸���ʷ:
 *
 *	2013-10-15 - �״δ���
 *
 *                     ����  <yl.tienon@gmal.com>
 */
#ifndef	__BITS_CONFIG_HEADER__
#define __BITS_CONFIG_HEADER__

/**
 *	���ͬʱ�����Ķ˿�����(���겻���޸�)
 */
#define	NR_LSNR_PORT	64
#define	CONFIG_VALUE_LENGTH	1024

struct gcs_config {
	unsigned short		lsnr_port[NR_LSNR_PORT];
	unsigned short		nr_lsnrs;		/*	��Ч�ļ����˿ڸ���*/
	unsigned short		core_ths;		/*	���Ĺ����߳�����*/
	unsigned short		conn_ths;		/*	���ӹ����߳�����*/
	unsigned short		nr_links;		/*	���������*/
	unsigned int		bytes_per_link;	/*	ÿ����·�Ļ�������С*/
	unsigned short		log_level;		/*	��־�ȼ�*/
	unsigned short		rst_close;		/*	RST�ر���·*/
	unsigned short		reuseaddr;		/*	��ַ�ذ�*/
	unsigned short		timeout;		/*	��ʱ���*/
};

#define	MAX_DOMAIN_LENGTH	255
struct gcs_nat {
	char	*src_ptr;
	char	*dst_ptr;
	char	src_addr[MAX_DOMAIN_LENGTH + 1];	/*	Դ��ַ*/
	char	dst_addr[MAX_DOMAIN_LENGTH + 1];	/*	Ŀ���ַ*/
	unsigned short src_port;					/*	Դ�˿�*/
	unsigned short dst_port;					/*	Ŀ��˿�*/
};

#endif	/*	__BITS_CONFIG_HEADER__*/