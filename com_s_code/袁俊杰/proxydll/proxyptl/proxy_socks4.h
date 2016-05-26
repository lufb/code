/**
 *	proxyptl/proxy_socks4.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	������socks4���������
 *
 *	�޸���ʷ:
 *
 *	2013-01-07 - �״δ���
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_PROXY_SOCKS4_H_
#define		_PROXY_SOCKS4_H_

#include "comm.h"


#pragma pack(1)

/**
		Ŀ��ת�����������ݽṹ��
					+----+----+----+----+----+----+----+----+----+----+....+----+
		�ֶΣ�		| VN | CD | DSTPORT |       DSTIP       |    USERID    |NULL|
					+----+----+----+----+----+----+----+----+----+----+....+----+
		�ֽ�����	| 1  | 1  |    2    |         4         |   variable   | 1  |
					+----+----+----+----+----+----+----+----+----+----+....+----+

 */
typedef struct _addr4
{
	char			ver;
	char			cd;
	short			dstport;
	long			dstip;
} addr4_req, addr4_res;

#pragma pack()

/**
 *	socks4_destination_transfer		- ��Ŀ���������������ͨ��
 *
 *	@sockfd			[in]�����������׽���
 *	@dst_addr		[in]��������ַ
 *	@dst_port		[in]�������˿�
 *
 *	return:
 *			==	0	�ɹ�
 *			!=	0	�����룬��Ӧerrorcode.h�ж���Ĵ���ԭ��
 */
int socks4_destination_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port );

#endif	// _PROXY_SOCKS4_H_