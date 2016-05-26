/**
 *	proxyptl/proxy_socks4.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了socks4代理处理过程
 *
 *	修改历史:
 *
 *	2013-01-07 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_PROXY_SOCKS4_H_
#define		_PROXY_SOCKS4_H_

#include "comm.h"


#pragma pack(1)

/**
		目标转换，请求数据结构：
					+----+----+----+----+----+----+----+----+----+----+....+----+
		字段：		| VN | CD | DSTPORT |       DSTIP       |    USERID    |NULL|
					+----+----+----+----+----+----+----+----+----+----+....+----+
		字节数：	| 1  | 1  |    2    |         4         |   variable   | 1  |
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
 *	socks4_destination_transfer		- 与目标服务器建立连接通道
 *
 *	@sockfd			[in]到服务器的套接字
 *	@dst_addr		[in]服务器地址
 *	@dst_port		[in]服务器端口
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int socks4_destination_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port );

#endif	// _PROXY_SOCKS4_H_