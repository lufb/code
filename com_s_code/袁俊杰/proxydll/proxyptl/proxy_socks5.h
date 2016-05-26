/**
 *	proxyptl/proxy_socks5.h
 *
 *	Copyright (C) 2012 yjj  <ayjj_8109@qq.com>
 *
 *	定义了socks5代理处理过程
 *
 *	修改历史:
 *
 *	2013-01-07 - 首次创建
 *
 *                     yjj	<ayjj_8109@qq.com>
 */

#ifndef		_PROXY_SOCKS5_H_
#define		_PROXY_SOCKS5_H_

#include "comm.h"


enum
{
	NOTNEED,						// X'00'       		NO AUTHENTICATION REQUIRED(无需认证)
	GSSAPI,							// X'01'        	GSSAPI
	USER_PWD,						// X'02'        	USERNAME/PASSWORD(用户名/口令认证机制)
	IANA_ASSIGNED_S		= 0x03,		// X'03'-X'7F'		IANA ASSIGNED
	IANA_ASSIGNED_E		= 0x7F,		// 
	PRIVATE_METHODS_S	= 0x80,		// X'80'-X'FE'		RESERVED FOR PRIVATE METHODS(私有认证机制)
	PRIVATE_METHODS_E	= 0xFE,		// 
	NO_ACCEPTABLE		= 0xFF,		// X'FF'        	NO ACCEPTABLE METHODS(完全不兼容)
};

#pragma pack(1)

/**
		目标转换，请求数据结构：
					+----+-----+-------+------+----------+----------+
		字段		|VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
					+----+-----+-------+------+----------+----------+
		字节数		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+

 */
typedef struct _addr_req
{
	char			ver;
	char			cmd;
	char			rsv;
	char			atyp;
} addr_req;

/**
		目标转换，应答数据结构：
					+----+-----+-------+------+----------+----------+
		字段		|VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
					+----+-----+-------+------+----------+----------+
		字节数		| 1  |  1  | X'00' |  1   | Variable |    2     |
					+----+-----+-------+------+----------+----------+
 */
typedef struct _addr_res
{
	char			ver;
	char			rep;
	char			rsv;
	char			atyp;
} addr_res;

#pragma pack()

/**
 *	socks5_authentication_negotiate		- 与代理服务器进行认证协商
 *
 *	@sockfd			[in]到服务器的套接字
 *	@auth			[out]代理服务器返回的认证方式
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int socks5_authentication_negotiate( IN int sockfd, OUT char* auth );

/**
 *	socks5_granted_authenticate			- 与代理服务器进行授权认证
 *
 *	@sockfd			[in]到服务器的套接字
 *	@auth			[in]认证授权方式
 *	@user			[in]用户名称
 *	@passwd			[in]认证密码
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int socks5_granted_authenticate( IN int sockfd, IN char auth, IN char* user, IN char* passwd );

/**
 *	socks5_destination_transfer		- 与目标服务器建立连接通道
 *
 *	@sockfd			[in]到服务器的套接字
 *	@dst_addr		[in]服务器地址
 *	@dst_port		[in]服务器端口
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int socks5_destination_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port );

/**
 * _auth_user_passwd	- 用户密码认证过程
 *
 *	@sockfd			[in]到服务器的套接字
 *	@user			[in]用户名称
 *	@passwd			[in]认证密码
 *
 *	return:
 *			==	0	成功
 *			!=	0	错误码，对应errorcode.h中定义的错误原因
 */
int _auth_user_passwd( IN int sockfd, IN char* user, IN char* passwd );

#endif		// _PROXY_SOCKS5_H_
