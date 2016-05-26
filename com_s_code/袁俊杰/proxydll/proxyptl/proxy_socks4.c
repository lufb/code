/**
 *	proxyptl/proxy_socks4.c
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

#include "proxy_socks4.h"

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
int socks4_destination_transfer( IN int sockfd, IN char* dst_addr, IN unsigned short dst_port )
{
	char			buf[256] = {0}, IP[16] = {0};
	int				res, offset;
	addr4_req*		add4req	= (addr4_req*)buf;
	addr4_res*		add4res = (addr4_res*)buf;

	if ( NULL == dst_addr )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_DST_NULL );
	}

	/*
		发送请求：
					+----+----+----+----+----+----+----+----+----+----+....+----+
		字段：		| VN | CD | DSTPORT |       DSTIP       |    USERID    |NULL|
					+----+----+----+----+----+----+----+----+----+----+....+----+
		字节数：	| 1  | 1  |    2    |         4         |   variable   | 1  |
					+----+----+----+----+----+----+----+----+----+----+....+----+

					VN			SOCKS协议版本号，这里必须填4
					CD			SOCKS的命令代码，对于建立连接的情况下，应该填1
					DSTPORT		目标服务器端口（2字节，网络字节序）
					DSTIP		目标服务器IP（4字节，网络字节序）
					USERID		用户ID（字符串，字节数不定）
					NULL		用户名称字符串的结束符，数值0
	*/

	get_host_by_name( dst_addr, IP );
	add4req->ver			= (char)0x04;				// 对于版本4这里是0x04
	add4req->cd				= (char)0x01;				// CONNECT
	add4req->dstport		= htons( dst_port );		// 目标端口
	if ( '\0' != IP[0] )								// 目标ip
		add4req->dstip		= inet_addr( IP );			
	else
		add4req->dstip		= inet_addr( dst_addr );

	offset					= sizeof(addr4_req);
	*(buf + offset)			= '\0';						// 没有用户id，最后必须填NULL
	offset					+= 1;
	
	res = single_select_and_send( sockfd, buf, offset, 5, 0 );
	if ( 0 != res )	return res;

	/*
		接收响应：
			字段：		+----+----+----+----+----+----+----+----+
						| VN | CD | DSTPORT |      DSTIP        |
						+----+----+----+----+----+----+----+----+
			字节数：	| 1  | 1  |   2     |        4			|
						+----+----+----+----+----+----+----+----+

			各字段含义：
						VN			响应代码版本号，这里必须是0
						CD			校验返回结果它的取值如下：
									90：授权代理
									91：拒绝服务或失败
									92：拒绝服务，由于代理服务器无法连接客户端认证
									93：拒绝服务，客户端和认证提供的用户id不一致
						DSTPORT		目标服务器端口（可以忽略）
						DSTIP		目标服务器IP（可以忽略）
	 */
	memset( buf, 0, sizeof(addr4_res) );
	res = single_select_and_recv( sockfd, (char*)add4res, sizeof(addr4_res), 5, 0 );
	if ( 0 != res )		return res;

	// 校验收到的协议
	if ( (char)90 != add4res->cd )
	{
		closesocket( sockfd );
		return _MKERRNO( 0, ERR_SERVICE_REJ );
	}

	return	0;
}